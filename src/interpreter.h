//
// Created by chrku on 10.08.2021.
//

#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include "types.h"
#include "expressions.h"
#include "statements.h"
#include "environment.h"

#include <vector>
#include <string_view>

class LoxInterpreter;

/*!
 * Represents runtime errors in Lox
 * E.g. Type errors, etc.
 */
class RuntimeError : public std::runtime_error {
public:
    /*!
     *
     * @param t Token where the error occurred
     * @param message Error message
     */
    RuntimeError(Token t, const std::string& message);

    /*!
     * Get the token responsible for the error
     * @return The token
     */
    [[nodiscard]] const Token& getToken() const;
private:
    Token token_;
};

class Interpreter : public ExpressionVisitor, public StatementVisitor {
public:
    /*!
     *
     */
    Interpreter();

    /*!
     *
     * @param program
     */
    void interpret(std::vector<std::unique_ptr<Statement>>& program,
                   const std::shared_ptr<LoxInterpreter>& context);

    /*!
     * Evaluate a Lox expression
     * @param expr Expression to be evaluated
     * @return Returned value, a Lox type (double, bool, null or string)
     */
    LoxType evaluate(Expression& expr);

    /*!
     *
     * @param statement
     */
    void execute(Statement& statement);

    ~Interpreter() override = default;
private:
    std::vector<LoxType> valueStack_;
    std::shared_ptr<Environment> environment_;

    void visitBinary(Binary &b) override;
    void visitTernary(Ternary &t) override;
    void visitGrouping(Grouping &g) override;
    void visitLiteral(Literal &l) override;
    void visitUnary(Unary &u) override;
    void visitVariableAccess(VariableAccess& v) override;
    void visitAssignment(Assignment& a) override;

    void visitExpressionStatement(ExpressionStatement& s) override;
    void visitPrintStatement(PrintStatement& p) override;
    void visitVariableDeclaration(VariableDeclaration& v) override;
    void visitBlock(Block& b) override;

    [[nodiscard]] static bool isTruthy(const LoxType& t);
    [[nodiscard]] static double negate(const Token& op, const LoxType& t);
    [[nodiscard]] static double toDouble(const LoxType& t);
    [[nodiscard]] static bool isEqual(const LoxType& t1, const LoxType& t2);

    static void checkNumberOperands(const Token& op, const LoxType& t1, const LoxType& t2);
    void executeBlock(std::vector<std::unique_ptr<Statement>>& statements,
                      std::shared_ptr<Environment> new_environment);
};


#endif //LOX_INTERPRETER_H
