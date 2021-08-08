//
// Created by chrku on 08.08.2021.
//

#ifndef LOX_AST_H
#define LOX_AST_H

#include "token.h"

#include <memory>
#include <utility>

class NullType {};

class Binary;
class Grouping;
class Literal;
class Unary;
class Expression;

class ExpressionVisitor {
public:
    virtual void visitBinary(Binary& b) = 0;
    virtual void visitGrouping(Grouping& g) = 0;
    virtual void visitLiteral(Literal& l) = 0;
    virtual void visitUnary(Unary& u) = 0;
};

class Expression {
public:
    virtual ~Expression() = default;
    virtual void accept (ExpressionVisitor& visitor) = 0;
};

class Binary : public Expression {
public:
    Binary(std::unique_ptr<Expression> left, Token  op, std::unique_ptr <Expression> right)
            : left_(std::move(left)), operator_(std::move(op)), right_(std::move(right)) {}

    ~Binary() override = default;

    void accept (ExpressionVisitor& visitor) override {
        visitor.visitBinary(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getLeft() const {
        return left_;
    }

    [[nodiscard]] const Token& getOperator() const {
        return operator_;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getRight() const {
        return right_;
    }

private:
    std::unique_ptr<Expression> left_;
    Token operator_;
    std::unique_ptr<Expression> right_;
};

class Grouping : public Expression {
public:
    explicit Grouping(std::unique_ptr<Expression> expression) : expression_(std::move(expression)) {}

    ~Grouping() override = default;

    void accept (ExpressionVisitor& visitor) override {
        visitor.visitGrouping(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getExpression() const {
        return expression_;
    }
private:
    std::unique_ptr<Expression> expression_;
};

class Literal : public Expression {
public:
    explicit Literal() : value_(NullType{}) {}
    explicit Literal(double value) : value_(value) {}
    explicit Literal(const std::string& value) : value_(value) {}
    explicit Literal(bool value) : value_(value) {}

    ~Literal() override = default;

    void accept (ExpressionVisitor& visitor) override {
        return visitor.visitLiteral(*this);
    }

    [[nodiscard]] const std::variant<double, std::string, bool, NullType>& getValue() const {
        return value_;
    }
private:
    std::variant<double, std::string, bool, NullType> value_;
};

class Unary : public Expression {
public:
    Unary(Token  op, std::unique_ptr<Expression> right) : operator_(std::move(op)), right_(std::move(right)) {}

    ~Unary() override = default;

    void accept (ExpressionVisitor& visitor) override {
        return visitor.visitUnary(*this);
    }

    [[nodiscard]] const Token& getOperator() const {
        return operator_;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getRight() const {
        return right_;
    }
private:
    Token operator_;
    std::unique_ptr<Expression> right_;
};

#endif //LOX_AST_H
