//
// Created by chrku on 14.08.2021.
//

#ifndef LOX_STATEMENTS_H
#define LOX_STATEMENTS_H

#include <utility>
#include <vector>

#include "expressions.h"

class ExpressionStatement;
class PrintStatement;
class VariableDeclaration;
class Block;
class IfStatement;

class StatementVisitor {
public:
    virtual void visitExpressionStatement(ExpressionStatement& s) = 0;
    virtual void visitPrintStatement(PrintStatement& p) = 0;
    virtual void visitVariableDeclaration(VariableDeclaration& v) = 0;
    virtual void visitBlock(Block& b) = 0;
    virtual void visitIfStatement(IfStatement& i) = 0;

    virtual ~StatementVisitor() = default;
};

/*!
 * Abstract base class for statements
 */
class Statement {
public:
    virtual ~Statement() = default;

    virtual void accept(StatementVisitor& visitor) = 0;
};

class VariableDeclaration : public Statement {
public:
    VariableDeclaration(std::unique_ptr<Expression> expression, Token token) : expression_(std::move(expression)),
                                                                                      token_(std::move(token)) {}

    ~VariableDeclaration() override = default;

    void accept(StatementVisitor& visitor) override {
        visitor.visitVariableDeclaration(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getExpression() const {
        return expression_;
    }

    [[nodiscard]] const Token& getToken() const {
        return token_;
    }

private:
    std::unique_ptr<Expression> expression_;
    Token token_;
};

class ExpressionStatement : public Statement {
public:
    explicit ExpressionStatement(std::unique_ptr<Expression> expression) : expression_(std::move(expression)) {}

    ~ExpressionStatement() override = default;

    void accept(StatementVisitor& visitor) override {
        visitor.visitExpressionStatement(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getExpression() const {
        return expression_;
    }

private:
    std::unique_ptr<Expression> expression_;
};

class PrintStatement : public Statement {
public:
    explicit PrintStatement(std::unique_ptr<Expression> expression) : expression_(std::move(expression)) {}

    ~PrintStatement() override = default;

    void accept(StatementVisitor& visitor) override {
        visitor.visitPrintStatement(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getExpression() const {
        return expression_;
    }

private:
    std::unique_ptr<Expression> expression_;
};

class Block : public Statement {
public:
    explicit Block(std::vector<std::unique_ptr<Statement>>&& statements) : statements_(std::move(statements)) {}

    ~Block() override = default;

    void accept(StatementVisitor& visitor) override {
        visitor.visitBlock(*this);
    }

    [[nodiscard]] std::vector<std::unique_ptr<Statement>>& getStatements() {
        return statements_;
    }

private:
    std::vector<std::unique_ptr<Statement>> statements_;
};

class IfStatement : public Statement {
public:
    IfStatement(std::unique_ptr<Expression> condition,
                std::unique_ptr<Statement> thenBranch,
                std::unique_ptr<Statement> elseBranch)
                : condition_(std::move(condition)),
                  thenBranch_(std::move(thenBranch)),
                  elseBranch_(std::move(elseBranch)) {}

    ~IfStatement() override = default;

    void accept(StatementVisitor& visitor) override {
        visitor.visitIfStatement(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getCondition() const {
        return condition_;
    }

    [[nodiscard]] const std::unique_ptr<Statement>& getThenBranch() const {
        return thenBranch_;
    }

    [[nodiscard]] const std::unique_ptr<Statement>& getElseBranch() const {
        return elseBranch_;
    }

private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Statement> thenBranch_;
    std::unique_ptr<Statement> elseBranch_;
};

#endif //LOX_STATEMENTS_H
