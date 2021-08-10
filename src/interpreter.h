//
// Created by chrku on 10.08.2021.
//

#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include "ast.h"

#include <vector>
#include <string_view>

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(Token t, const std::string& message);
    [[nodiscard]] const Token& getToken() const;
private:
    Token token_;
};

class Interpreter : public ExpressionVisitor {
public:
    LoxType evaluate(Expression& expr);
private:
    std::vector<LoxType> valueStack_;

    void visitBinary(Binary &b) override;
    void visitTernary(Ternary &t) override;
    void visitGrouping(Grouping &g) override;
    void visitLiteral(Literal &l) override;
    void visitUnary(Unary &u) override;

    [[nodiscard]] static bool isTruthy(const LoxType& t);
    [[nodiscard]] static double negate(const Token& op, const LoxType& t);
    [[nodiscard]] static double toDouble(const LoxType& t);
    [[nodiscard]] static bool isEqual(const LoxType& t1, const LoxType& t2);

    static void checkNumberOperands(const Token& op, const LoxType& t1, const LoxType& t2) ;
};


#endif //LOX_INTERPRETER_H
