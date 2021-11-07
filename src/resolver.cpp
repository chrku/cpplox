//
// Created by Christoph Kummer on 04.11.21.
//

#include "resolver.h"
#include "lox.h"

Resolver::Resolver(std::shared_ptr<Interpreter> interpreter,
                   std::shared_ptr<LoxInterpreter> context)
    : interpreter_{std::move(interpreter)}, context_{std::move(context)}, scopes_{}
{}

void Resolver::visitBinary(Binary& b) {

}

void Resolver::visitTernary(Ternary& t) {

}

void Resolver::visitGrouping(Grouping& g) {

}

void Resolver::visitLiteral(Literal& l) {

}

void Resolver::visitUnary(Unary& u) {

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

    resolveLocal(v);
}

void Resolver::visitAssignment(Assignment& a) {

}

void Resolver::visitLogical(Logical& l) {

}

void Resolver::visitCall(Call& c) {

}

void Resolver::visitFunctionExpression(FunctionExpression& f) {

}

void Resolver::visitExpressionStatement(ExpressionStatement& s) {

}

void Resolver::visitPrintStatement(PrintStatement& p) {

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

}

void Resolver::visitWhileStatement(WhileStatement& w) {

}

void Resolver::visitBreakStatement(BreakStatement& b) {

}

void Resolver::visitFunction(Function& f) {

}

void Resolver::visitReturn(Return& r) {

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
    scope[name.getLexeme()] = false;
}

void Resolver::define(const Token& name) {
    if (scopes_.empty()) { return; }
    auto& scope = scopes_.back();
    scope[name.getLexeme()] = true;
}

void Resolver::resolveLocal(Expression* expr, const std::string& name) {
    for (auto i = scopes_.size(); i >= 0; --i) {
        if (scopes_[i].count(name)) {
            interpreter_->resolve(expr, static_cast<int>(i));
        }
    }
}

