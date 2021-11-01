//
// Created by chrku on 14.08.2021.
//

#ifndef LOX_EXPRESSIONS_H
#define LOX_EXPRESSIONS_H

#include <vector>


class Binary;
class Ternary;
class Grouping;
class Literal;
class Unary;
class VariableAccess;
class Assignment;
class Logical;
class Call;
class FunctionExpression;

class Statement;

/*!
 * AST visitor interface
 */
class ExpressionVisitor {
public:
    virtual void visitBinary(Binary& b) = 0;
    virtual void visitTernary(Ternary& t) = 0;
    virtual void visitGrouping(Grouping& g) = 0;
    virtual void visitLiteral(Literal& l) = 0;
    virtual void visitUnary(Unary& u) = 0;
    virtual void visitVariableAccess(VariableAccess& v) = 0;
    virtual void visitAssignment(Assignment& a) = 0;
    virtual void visitLogical(Logical& l) = 0;
    virtual void visitCall(Call& c) = 0;
    virtual void visitFunctionExpression(FunctionExpression& f) = 0;

    virtual ~ExpressionVisitor() = default;
};

/*!
 * Abstract base class for expressions
 */
class Expression {
public:
    virtual ~Expression() = default;
    virtual void accept (ExpressionVisitor& visitor) = 0;
    [[nodiscard]] virtual bool lvalue() const { return false; }
};

/*!
 * Expressions with arity two
 */
class Binary : public Expression {
public:
    Binary(std::unique_ptr<Expression> left, Token  op, std::unique_ptr<Expression> right)
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

/*!
 * Represents ternary conditional operator
 */
class Ternary : public Expression {
public:
    Ternary(std::unique_ptr<Expression> left, std::unique_ptr<Expression> middle, std::unique_ptr<Expression> right)
        : left_(std::move(left)), middle_(std::move(middle)), right_(std::move(right)) {}

    ~Ternary() override = default;

    void accept(ExpressionVisitor& visitor) override {
        visitor.visitTernary(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getLeft() const {
        return left_;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getMiddle() const {
        return middle_;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getRight() const {
        return right_;
    }

private:
    std::unique_ptr<Expression> left_;
    std::unique_ptr<Expression> middle_;
    std::unique_ptr<Expression> right_;
};

/*!
 * Expressions in parentheses
 */
class Grouping : public Expression {
public:
    explicit Grouping(std::unique_ptr<Expression> expression) : expression_(std::move(expression)) {}

    ~Grouping() override = default;

    void accept(ExpressionVisitor& visitor) override {
        visitor.visitGrouping(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getExpression() const {
        return expression_;
    }
private:
    std::unique_ptr<Expression> expression_;
};

/*!
 * Literal expressions
 */
class Literal : public Expression {
public:
    explicit Literal() : value_(NullType{}) {}
    explicit Literal(double value) : value_(value) {}
    explicit Literal(const std::string& value) : value_(value) {}
    explicit Literal(bool value) : value_(value) {}

    ~Literal() override = default;

    void accept(ExpressionVisitor& visitor) override {
        return visitor.visitLiteral(*this);
    }

    [[nodiscard]] const LoxType& getValue() const {
        return value_;
    }
private:
    LoxType value_;
};

/*!
 * Unary expressions
 */
class Unary : public Expression {
public:
    Unary(Token op, std::unique_ptr<Expression> right) : operator_(std::move(op)), right_(std::move(right)) {}

    ~Unary() override = default;

    void accept(ExpressionVisitor& visitor) override {
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

/*!
 * Variable access expressions
 */
class VariableAccess : public Expression {
public:
    explicit VariableAccess(Token name) : name_(std::move(name)) {}

    ~VariableAccess() override = default;

    void accept(ExpressionVisitor& visitor) override {
        return visitor.visitVariableAccess(*this);
    }

    [[nodiscard]] const Token& getToken() {
        return name_;
    }

    [[nodiscard]] bool lvalue() const override {
        return true;
    }

private:
    Token name_;
};

/*!
 * Assignment expressions
 */
class Assignment : public Expression {
public:
    Assignment(Token name, std::unique_ptr<Expression> expression)
        : name_(std::move(name)), value_(std::move(expression)) {}

    ~Assignment() override = default;

    void accept(ExpressionVisitor& visitor) override {
        return visitor.visitAssignment(*this);
    }

    [[nodiscard]] const Token& getName() const {
        return name_;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getValue() const {
        return value_;
    }

private:
    Token name_;
    std::unique_ptr<Expression> value_;
};

class Logical : public Expression {
public:
    Logical(std::unique_ptr<Expression> left, const Token& token, std::unique_ptr<Expression> right)
        : left_(std::move(left)), operator_(token), right_(std::move(right)) {}

    ~Logical() override = default;

    void accept(ExpressionVisitor& visitor) override {
        return visitor.visitLogical(*this);
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

class Call : public Expression {
public:
    Call(std::unique_ptr<Expression> callee, const Token& paren, std::vector<std::unique_ptr<Expression>>&& arguments)
        : callee_(std::move(callee)), paren_(paren), arguments_(std::move(arguments)) {}

    ~Call() override = default;

    void accept(ExpressionVisitor& visitor) override {
        return visitor.visitCall(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression> &getCallee() const {
        return callee_;
    }

    [[nodiscard]] const Token &getParen() const {
        return paren_;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<Expression>> &getArguments() const {
        return arguments_;
    }

private:
    std::unique_ptr<Expression> callee_;
    Token paren_;
    std::vector<std::unique_ptr<Expression>> arguments_;
};

class FunctionExpression : public Expression {
public:
    FunctionExpression(const std::vector<Token>& arguments, std::vector<std::shared_ptr<Statement>>  statements)
        : params_(arguments), body_(std::move(statements)) {}

    ~FunctionExpression() override = default;

    void accept(ExpressionVisitor& visitor) override {
        return visitor.visitFunctionExpression(*this);
    }

    [[nodiscard]] const std::vector<Token>& getParams() const {
        return params_;
    }

    [[nodiscard]] const std::vector<std::shared_ptr<Statement>>& getBody() const {
        return body_;
    }

private:
    std::vector<Token> params_;
    std::vector<std::shared_ptr<Statement>> body_;
};

#endif //LOX_EXPRESSIONS_H
