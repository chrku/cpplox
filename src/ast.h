//
// Created by chrku on 08.08.2021.
//

#ifndef LOX_AST_H
#define LOX_AST_H

#include "token.h"

#include <memory>

class Expression {
public:
    virtual ~Expression() = default;
};

class NullType {};

class Binary;
class Grouping;
class Literal;
class Unary;

template<typename T>
class ExpressionVisitor {
public:
    T visitBinary(Binary& b) = 0;
    T visitGrouping(Grouping& g) = 0;
    T visitLiteral(Literal& l) = 0;
    T visitUnary(Unary& u) = 0;
};

class Binary : public Expression {
public:
    Binary(std::unique_ptr<Expression> left, const Token& op, std::unique_ptr <Expression> right)
            : left_(std::move(left)), operator_(op), right_(std::move(right)) {}

    ~Binary() override = default;

    template<typename T>
    T accept (const ExpressionVisitor<T>& visitor) {
        return visitor.visitBinary(*this);
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

    template<typename T>
    T accept (const ExpressionVisitor<T>& visitor) {
        return visitor.visitGrouping(*this);
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

    template<typename T>
    T accept (const ExpressionVisitor<T>& visitor) {
        return visitor.visitLiteral(*this);
    }
private:
    std::variant<double, std::string, bool, NullType> value_;
};

class Unary : public Expression {
public:
    Unary(const Token& op, std::unique_ptr<Expression> right) : operator_(op), right_(std::move(right)) {}
    ~Unary() override = default;

    template<typename T>
    T accept (const ExpressionVisitor<T>& visitor) {
        return visitor.visitUnary(*this);
    }
private:
    Token operator_;
    std::unique_ptr<Expression> right_;
};

#endif //LOX_AST_H
