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
class WhileStatement;
class BreakStatement;
class Function;

class StatementVisitor {
public:
    virtual void visitExpressionStatement(ExpressionStatement& s) = 0;
    virtual void visitPrintStatement(PrintStatement& p) = 0;
    virtual void visitVariableDeclaration(VariableDeclaration& v) = 0;
    virtual void visitBlock(Block& b) = 0;
    virtual void visitIfStatement(IfStatement& i) = 0;
    virtual void visitWhileStatement(WhileStatement& w) = 0;
    virtual void visitBreakStatement(BreakStatement& b) = 0;
    virtual void visitFunction(Function& f) = 0;

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
    explicit Block(std::vector<std::shared_ptr<Statement>>& statements) : statements_(statements) {}

    ~Block() override = default;

    void accept(StatementVisitor& visitor) override {
        visitor.visitBlock(*this);
    }

    [[nodiscard]] std::vector<std::shared_ptr<Statement>>& getStatements() {
        return statements_;
    }

private:
    std::vector<std::shared_ptr<Statement>> statements_;
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

class WhileStatement : public Statement {
public:
    WhileStatement(std::unique_ptr<Expression> condition,
                   std::unique_ptr<Statement> thenBranch)
                   : condition_(std::move(condition)),
                   thenBranch_(std::move(thenBranch)) {}

   ~WhileStatement() override = default;

    void accept(StatementVisitor& visitor) override {
        visitor.visitWhileStatement(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getCondition() const {
        return condition_;
    }

    [[nodiscard]] const std::unique_ptr<Statement>& getThenBranch() const {
        return thenBranch_;
    }

private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Statement> thenBranch_;
};

class BreakStatement : public Statement {
public:
    BreakStatement() {}

    ~BreakStatement() override = default;

    void accept(StatementVisitor& visitor) override {
        visitor.visitBreakStatement(*this);
    }
};

class Function : public Statement {
public:
    Function(const Token& name, const std::vector<Token>& params, std::vector<std::shared_ptr<Statement>>& body)
    : name_(name), params_(params), body_(body)
    {}

    ~Function() override = default;

    void accept(StatementVisitor& visitor) override {
        visitor.visitFunction(*this);
    }

    [[nodiscard]] const Token& getName() const {
        return name_;
    }

    [[nodiscard]] const std::vector<Token>& getParams() const {
        return params_;
    }

    [[nodiscard]] const std::vector<std::shared_ptr<Statement>>& getBody() const {
        return body_;
    }

private:
    Token name_;
    std::vector<Token> params_;
    std::vector<std::shared_ptr<Statement>> body_;
};

#endif //LOX_STATEMENTS_H
