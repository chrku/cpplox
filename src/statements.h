//
// Created by chrku on 14.08.2021.
//

#ifndef LOX_STATEMENTS_H
#define LOX_STATEMENTS_H

#include "expressions.h"

class ExpressionStatement;
class PrintStatement;

class StatementVisitor {
public:
    virtual void visitExpressionStatement(ExpressionStatement& s) = 0;
    virtual void visitPrintStatement(PrintStatement& p) = 0;

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

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(std::unique_ptr<Expression> expression) : expression_(std::move(expression)) {}

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
    PrintStatement(std::unique_ptr<Expression> expression) : expression_(std::move(expression)) {}

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

#endif //LOX_STATEMENTS_H
