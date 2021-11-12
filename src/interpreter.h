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
#include <unordered_map>
#include <ostream>

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

class BreakException : public std::exception {
public:
    BreakException() = default;
};

class ReturnException : public std::exception {
public:
    explicit ReturnException(const LoxType& value);

    [[nodiscard]] const LoxType& getValue() const;
private:
    LoxType value_;
};

class Interpreter : public ExpressionVisitor, public StatementVisitor {
public:
    /*!
     * Construct interpreter instance, initialize empty environment
     */
    Interpreter();

    Interpreter(std::ostream* ostream, bool test_mode);

    /*!
     * Interpret lox program
     * @param program sequence of statements
     * @param context interpreter context for error reporting
     */
    void interpret(std::vector<std::shared_ptr<Statement>>& program,
                   const std::shared_ptr<LoxInterpreter>& context);

    /*!
     * Evaluate a Lox expression
     * @param expr Expression to be evaluated
     * @return Returned value, a Lox type (double, bool, null or string)
     */
    LoxType evaluate(Expression& expr);

    /*!
     * Execute a lox statement
     * @param statement statement to execute
     */
    void execute(Statement& statement);

    ~Interpreter() override = default;

    [[nodiscard]] const std::shared_ptr<Environment>& getEnvironment() const;

    void executeBlock(const std::vector<std::shared_ptr<Statement>>& statements,
                      std::shared_ptr<Environment> new_environment);

    void resolve(Expression* expr, int depth);
private:
    std::vector<LoxType> valueStack_;
    std::shared_ptr<Environment> globals_;
    std::shared_ptr<Environment> environment_;

    std::unordered_map<Expression*, int> locals_;

    bool testMode_;
    std::ostream* outputStream_;

    void visitBinary(Binary &b) override;
    void visitTernary(Ternary &t) override;
    void visitGrouping(Grouping &g) override;
    void visitLiteral(Literal &l) override;
    void visitUnary(Unary &u) override;
    void visitVariableAccess(VariableAccess& v) override;
    void visitAssignment(Assignment& a) override;
    void visitLogical(Logical& l) override;
    void visitCall(Call& c) override;
    void visitFunctionExpression(FunctionExpression& f) override;

    void visitExpressionStatement(ExpressionStatement& s) override;
    void visitPrintStatement(PrintStatement& p) override;
    void visitVariableDeclaration(VariableDeclaration& v) override;
    void visitBlock(Block& b) override;
    void visitIfStatement(IfStatement& i) override;
    void visitWhileStatement(WhileStatement& w) override;
    void visitBreakStatement(BreakStatement& b) override;
    void visitFunction(Function& f) override;
    void visitReturn(Return& r) override;

    [[nodiscard]] static bool isTruthy(const LoxType& t);
    [[nodiscard]] static double negate(const Token& op, const LoxType& t);
    [[nodiscard]] static double toDouble(const LoxType& t);
    [[nodiscard]] static bool isEqual(const LoxType& t1, const LoxType& t2);

    static void checkNumberOperands(const Token& op, const LoxType& t1, const LoxType& t2);
    LoxType lookUpVariable(const Token& name, Expression* expr);
};


#endif //LOX_INTERPRETER_H
