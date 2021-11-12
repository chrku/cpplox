//
// Created by Christoph Kummer on 04.11.21.
//

#ifndef LOX_RESOLVER_H
#define LOX_RESOLVER_H

#include "statements.h"
#include "interpreter.h"

#include <unordered_map>
#include <unordered_set>

class LoxInterpreter;

enum class FunctionType {
    NONE,
    FUNCTION
};

class Resolver : public ExpressionVisitor, public StatementVisitor {
public:
    Resolver(std::shared_ptr<Interpreter> interpreter,
             std::shared_ptr<LoxInterpreter> context);

    void resolve(std::vector<std::shared_ptr<Statement>>& statements);

    void visitBinary(Binary &b) override;
    void visitTernary(Ternary &t) override;
    void visitGrouping(Grouping &g) override;
    void visitLiteral(Literal &l) override;
    void visitUnary(Unary &u) override;
    void visitVariableAccess(VariableAccess &v) override;
    void visitAssignment(Assignment &a) override;
    void visitLogical(Logical &l) override;
    void visitCall(Call &c) override;

    void visitFunctionExpression(FunctionExpression &f) override;
    void visitExpressionStatement(ExpressionStatement &s) override;
    void visitPrintStatement(PrintStatement &p) override;
    void visitVariableDeclaration(VariableDeclaration &v) override;
    void visitBlock(Block &b) override;
    void visitIfStatement(IfStatement &i) override;
    void visitWhileStatement(WhileStatement &w) override;
    void visitBreakStatement(BreakStatement &b) override;
    void visitFunction(Function &f) override;
    void visitReturn(Return &r) override;

    ~Resolver() override = default;
private:
    std::shared_ptr<Interpreter> interpreter_;
    std::shared_ptr<LoxInterpreter> context_;
    std::vector<std::unordered_map<Token, bool>> scopes_;
    std::vector<std::unordered_set<Token>> usage_;
    FunctionType currentFunction_ = FunctionType::NONE;

    void beginScope();
    void endScope();

    void resolve(Expression& e);
    void resolve(Statement& s);

    void declare(const Token& name);
    void define(const Token& name);
    void resolveLocal(Expression* expr, const Token& name);

    void resolveFunction(Function &f, FunctionType type);
};


#endif //LOX_RESOLVER_H
