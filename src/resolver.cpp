//
// Created by Christoph Kummer on 04.11.21.
//

#include "token.h"
#include "resolver.h"
#include "lox.h"

Resolver::Resolver(std::shared_ptr<Interpreter> interpreter,
                   std::shared_ptr<LoxInterpreter> context)
    : interpreter_{std::move(interpreter)}, context_{std::move(context)}, scopes_{}
{}

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
        const auto& name = v.getToken().getLexeme();
        if (scope.count(name) && !scope[name]) {
            context_->error(v.getToken(),
                            "Can't read local variable in its own initializer");
        }
    }

    resolveLocal(&v, v.getToken().getLexeme());
}

void Resolver::visitAssignment(Assignment& a) {
    resolve(*a.getValue());
    resolveLocal(&a, a.getName().getLexeme());
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

void Resolver::visitFunctionExpression(FunctionExpression& f) {
    beginScope();
    for (const auto& param : f.getParams()) {
        declare(param);
        define(param);
    }
    resolve(f.getBody());
    endScope();
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

    beginScope();
    for (const auto& param : f.getParams()) {
        declare(param);
        define(param);
    }
    resolve(f.getBody());
    endScope();
}

void Resolver::visitReturn(Return& r) {
    resolve(*r.getValue());
}

void Resolver::beginScope() {
    scopes_.emplace_back();
}

void Resolver::endScope() {
    scopes_.pop_back();
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
    if (scope.count(name.getLexeme())) {
        context_->error(name,
                        "Already a variable with this name in this scope.");
    }
    scope[name.getLexeme()] = false;
}

void Resolver::define(const Token& name) {
    if (scopes_.empty()) { return; }
    auto& scope = scopes_.back();
    scope[name.getLexeme()] = true;
}

void Resolver::resolveLocal(Expression* expr, const std::string& name) {
    for (int i = static_cast<int>(scopes_.size()); i >= 0; --i) {
        if (scopes_[i].count(name)) {
            interpreter_->resolve(expr, static_cast<int>(i));
        }
    }
}

