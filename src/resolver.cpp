//
// Created by Christoph Kummer on 04.11.21.
//

#include "token.h"
#include "resolver.h"
#include "lox.h"
#include "native_functions/clock.h"

Resolver::Resolver(std::shared_ptr<Interpreter> interpreter,
                   std::shared_ptr<LoxInterpreter> context,
                   bool test_mode)
    : interpreter_{std::move(interpreter)}, context_{std::move(context)}, scopes_{}, usage_{}
{
    Token clockToken = Token(TokenType::IDENTIFIER, "clock", 0);
    defineGlobal(clockToken);
    interpreter_->defineGlobal(std::make_shared<Clock>(test_mode));
}

void Resolver::visitBinary(Binary& b) {
    resolve(*b.getLeft());
    resolve(*b.getRight());
}

void Resolver::visitTernary(Ternary& t) {
    resolve(*t.getLeft());
    resolve(*t.getMiddle());
    resolve(*t.getRight());
}

void Resolver::visitGrouping(Grouping& g) {
    resolve(*g.getExpression());
}

void Resolver::visitLiteral(Literal& l) {

}

void Resolver::visitUnary(Unary& u) {
    resolve(*u.getRight());
}

void Resolver::visitVariableAccess(VariableAccess& v) {
    if (!scopes_.empty()) {
        auto& scope = scopes_.back();
        const auto& name = v.getToken();
        if (scope.count(name) && !scope[name]) {
            context_->error(v.getToken(),
                            "Can't read local variable in its own initializer");
        }
    }

    resolveLocal(&v, v.getToken());

    if (!usage_.empty()) {
        for (int i = static_cast<int>(usage_.size()) - 1; i >= 0; --i) {
            auto &cur_usage = usage_[i];
            if (scopes_[i].count(v.getToken())) {
                cur_usage.insert(v.getToken());
            }
        }
    }
}

void Resolver::visitAssignment(Assignment& a) {
    resolve(*a.getValue());
    resolveLocal(&a, a.getName());
}

void Resolver::visitLogical(Logical& l) {
    resolve(*l.getLeft());
    resolve(*l.getRight());
}

void Resolver::visitCall(Call& c) {
    for (const auto& expr : c.getArguments()) {
        resolve(*expr);
    }
    resolve(*c.getCallee());
}

void Resolver::visitGetExpression(GetExpression& g) {
    resolve(*g.getObject());
}

void Resolver::visitSetExpression(SetExpression& s) {
    resolve(*s.getObject());
    resolve(*s.getValue());
}

void Resolver::visitFunctionExpression(FunctionExpression& f) {
    auto enclosing = currentFunction_;
    currentFunction_ = FunctionType::FUNCTION;

    beginScope();
    for (const auto& param : f.getParams()) {
        declare(param);
        define(param);
    }
    resolve(f.getBody());
    endScope();
    currentFunction_ = enclosing;
}

void Resolver::visitThisExpression(ThisExpression& t) {
    if (currentClass_ == ClassType::NONE) {
        context_->error(t.getKeyword(),
                        "Can't use 'this' outside of a class.");
        return;
    }
    resolveLocal(&t, t.getKeyword());
}

void Resolver::visitSuperExpression(SuperExpression& s) {
    resolveLocal(&s, s.getKeyword());
}

void Resolver::visitExpressionStatement(ExpressionStatement& s) {
    resolve(*s.getExpression());
}

void Resolver::visitPrintStatement(PrintStatement& p) {
    resolve(*p.getExpression());
}

void Resolver::visitVariableDeclaration(VariableDeclaration& v) {
    declare(v.getToken());
    if (v.getExpression()) {
        resolve(*v.getExpression());
    }
    define(v.getToken());
}

void Resolver::visitBlock(Block& b) {
    beginScope();
    resolve(b.getStatements());
    endScope();
}

void Resolver::visitIfStatement(IfStatement& i) {
    resolve(*i.getCondition());
    resolve(*i.getThenBranch());
    if (i.getElseBranch()) {
        resolve(*i.getElseBranch());
    }
}

void Resolver::visitWhileStatement(WhileStatement& w) {
    resolve(*w.getCondition());
    resolve(*w.getThenBranch());
}

void Resolver::visitBreakStatement(BreakStatement& b) {

}

void Resolver::visitFunction(Function& f) {
    declare(f.getName());
    define(f.getName());

    resolveFunction(f, FunctionType::FUNCTION);
}

void Resolver::resolveFunction(Function& f, FunctionType type) {
    auto enclosing = currentFunction_;
    currentFunction_ = type;

    beginScope();
    for (const auto& param : f.getParams()) {
        declare(param);
        define(param);
    }
    resolve(f.getBody());
    endScope();
    currentFunction_ = enclosing;
}

void Resolver::visitReturn(Return& r) {
    if (currentFunction_ == FunctionType::NONE) {
        context_->error(r.getKeyword(), "Can't return from top-level code.");
    }
    if (r.getValue() != nullptr && currentFunction_ == FunctionType::INITIALIZER) {
        context_->error(r.getKeyword(), "Can't return from initializer.");
    }
    resolve(*r.getValue());
}

void Resolver::visitClassDeclaration(ClassDeclaration& c) {
    ClassType curType = currentClass_;
    currentClass_ = ClassType::CLASS;

    declare(c.getName());

    if (c.getSuperclass() && c.getName().getLexeme() == c.getSuperclass()->getToken().getLexeme()) {
        context_->error(c.getSuperclass()->getToken(), "A class can't inherit from itself.");
    }

    if (c.getSuperclass()) {
        resolve(*c.getSuperclass());
    }

    if (c.getSuperclass()) {
        beginScope();
        Token super_token(TokenType::THIS, "super", -1);
        scopes_.back()[super_token] = true;
    }

    beginScope();
    Token this_token(TokenType::THIS, "this", -1);
    scopes_.back()[this_token] = true;

    for (const auto& method : c.getMethods()) {
        auto declaration = FunctionType::METHOD;
        if (method->getName().getLexeme() == "init") {
            declaration = FunctionType::INITIALIZER;
        }
        resolveFunction(*method, declaration);
    }

    endScope();

    if (c.getSuperclass()) {
        endScope();
    }

    define(c.getName());
    currentClass_ = curType;
}

void Resolver::beginScope() {
    scopes_.emplace_back();
    usage_.emplace_back();
    localLocations_.emplace_back();
    localIndexStack_.emplace_back(0);
}

void Resolver::endScope() {
    const auto &cur_usage_set = usage_.back();
    for (const auto &pair: scopes_.back()) {
        const auto &name = pair.first;
        if (name.getLexeme() != "this" && name.getLexeme() != "super" && !cur_usage_set.count(name)) {
            context_->error(name, "Local variable not used.");
        }
    }
    scopes_.pop_back();
    usage_.pop_back();
    localLocations_.pop_back();
    localIndexStack_.pop_back();
}

void Resolver::resolve(Expression& e) {
    e.accept(*this);
}

void Resolver::resolve(Statement& s) {
    s.accept(*this);
}

void Resolver::resolve(std::vector<std::shared_ptr<Statement>>& statements) {
    for (auto& statement_ptr : statements) {
        resolve(*statement_ptr);
    }
}

void Resolver::declare(const Token& name) {
    if (scopes_.empty()) { return; }
    auto& scope = scopes_.back();
    if (scope.count(name)) {
        context_->error(name,
                        "Already a variable with this name in this scope.");
    }
    scope[name] = false;
}

void Resolver::define(const Token& name) {
    if (scopes_.empty()) {
        defineGlobal(name);
        return;
    }
    auto& scope = scopes_.back();
    scope[name] = true;
    defineLocal(name);
}

void Resolver::resolveLocal(Expression* expr, const Token& name) {
    for (int i = static_cast<int>(scopes_.size()) - 1; i >= 0; --i) {
        if (scopes_[i].count(name)) {
            interpreter_->resolve(expr, localLocations_[i][name], scopes_.size() - i - 1);
            return;
        }
    }

    if (!globalLocations_.count(name)) {
        context_->error(name, "Undefined variable.");
    }
    interpreter_->resolve(expr, globalLocations_[name], GLOBAL_DEPTH);
}

void Resolver::defineGlobal(const Token& name) {
    auto location = globalIndex_++;
    globalLocations_[name] = location;
}

void Resolver::defineLocal(const Token& name) {
    auto& current_map = localLocations_.back();
    auto& index = localIndexStack_.back();
    auto location = index++;
    current_map[name] = location;
}




