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

/**
 * Exception used to re-synchronize when breaking out of loops
 */
class BreakException : public std::exception {
public:
    BreakException() = default;
};

/**
 * Exception used to re-synchronize when returning from functions
 */
class ReturnException : public std::exception {
public:
    explicit ReturnException(const LoxType& value);

    [[nodiscard]] const LoxType& getValue() const;
private:
    LoxType value_;
};

/**
 * Main Lox interpreter. Takes in AST and interprets it
 * after resolve pass
 */
class Interpreter : public ExpressionVisitor, public StatementVisitor {
public:
    /*!
     * Construct interpreter instance, initialize empty environment
     */
    Interpreter();

    /**
     * Constructor for testing, allows redirection of standard output to
     * alternate stream
     * @param ostream stream to redirect print statements to
     */
    explicit Interpreter(std::ostream *ostream);

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

    /**
     * Get current environment
     * @return reference to current environment
     */
    [[nodiscard]] const std::shared_ptr<Environment>& getEnvironment() const;

    /**
     * Execute block of statements, e.g. in a function
     * @param statements reference to block of statements
     * @param new_environment new environment to use for block
     */
    void executeBlock(const std::vector<std::shared_ptr<Statement>>& statements,
                      std::shared_ptr<Environment> new_environment);

    /**
     * During resolve pass, this tells the interpreter where to look for
     * local variables
     * @param expr location in AST
     * @param location location in scope array
     * @param depth distance from current scope for lookup
     */
    void resolve(Expression* expr, std::size_t location, std::size_t depth);

    /**
     * Assign value to global variable, which will be placed into globals array
     * Used to define native functions at interpreter startup by resolver
     * @param value value to initialize global to
     */
    void defineGlobal(LoxType value);
private:
    std::vector<LoxType> valueStack_;
    std::shared_ptr<Environment> globals_;
    std::shared_ptr<Environment> environment_;

    // Location of local variables, associates AST tree location with location in scope relative
    // to current scope
    std::unordered_map<Expression*, std::pair<std::size_t, std::size_t>> exprLocations_;

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
    void visitGetExpression(GetExpression& g) override;

    void visitExpressionStatement(ExpressionStatement& s) override;
    void visitPrintStatement(PrintStatement& p) override;
    void visitVariableDeclaration(VariableDeclaration& v) override;
    void visitBlock(Block& b) override;
    void visitIfStatement(IfStatement& i) override;
    void visitWhileStatement(WhileStatement& w) override;
    void visitBreakStatement(BreakStatement& b) override;
    void visitFunction(Function& f) override;
    void visitReturn(Return& r) override;
    void visitClassDeclaration(ClassDeclaration& c) override;

    [[nodiscard]] static bool isTruthy(const LoxType& t);
    [[nodiscard]] static double negate(const Token& op, const LoxType& t);
    [[nodiscard]] static double toDouble(const LoxType& t);
    [[nodiscard]] static bool isEqual(const LoxType& t1, const LoxType& t2);

    static void checkNumberOperands(const Token& op, const LoxType& t1, const LoxType& t2);
    LoxType lookUpVariable(Expression* expr);
};


#endif //LOX_INTERPRETER_H
