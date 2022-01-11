//
// Created by Christoph Kummer on 04.11.21.
//

#ifndef LOX_RESOLVER_H
#define LOX_RESOLVER_H

#include "statements.h"
#include "interpreter.h"

#include <unordered_map>
#include <unordered_set>

class LoxInterpreter;

/**
 * Represents different types of functions,
 * e.g. methods and normal functions
 */
enum class FunctionType {
    NONE,
    FUNCTION,
    METHOD
};

/**
 * This class does the resolve pass,
 * which maps variable names to their locations
 */
class Resolver : public ExpressionVisitor, public StatementVisitor {
public:
    /**
     * Constructor
     * @param interpreter reference to interpreter
     * @param context reference to runtime for error reporting
     * @param test_mode test mode, for making non-deterministic functions
     * behave deterministically
     */
    Resolver(std::shared_ptr<Interpreter> interpreter,
             std::shared_ptr<LoxInterpreter> context,
             bool test_mode);

    /**
     * Resolve program, used by runtime to execute resolve pass
     * @param statements program
     */
    void resolve(std::vector<std::shared_ptr<Statement>>& statements);

    void visitBinary(Binary &b) override;
    void visitTernary(Ternary &t) override;
    void visitGrouping(Grouping &g) override;
    void visitLiteral(Literal &l) override;
    void visitUnary(Unary &u) override;
    void visitVariableAccess(VariableAccess &v) override;
    void visitAssignment(Assignment &a) override;
    void visitLogical(Logical &l) override;
    void visitCall(Call &c) override;
    void visitGetExpression(GetExpression& g) override;
    void visitSetExpression(SetExpression& s) override;
    void visitThisExpression(ThisExpression& t) override;

    void visitFunctionExpression(FunctionExpression &f) override;
    void visitExpressionStatement(ExpressionStatement &s) override;
    void visitPrintStatement(PrintStatement &p) override;
    void visitVariableDeclaration(VariableDeclaration &v) override;
    void visitBlock(Block &b) override;
    void visitIfStatement(IfStatement &i) override;
    void visitWhileStatement(WhileStatement &w) override;
    void visitBreakStatement(BreakStatement &b) override;
    void visitFunction(Function &f) override;
    void visitReturn(Return &r) override;
    void visitClassDeclaration(ClassDeclaration& c) override;

    ~Resolver() override = default;

    /**
     * Marker value for expressions in global scope
     */
    constexpr static std::size_t GLOBAL_DEPTH = std::numeric_limits<std::size_t>::max();
private:
    std::shared_ptr<Interpreter> interpreter_;
    std::shared_ptr<LoxInterpreter> context_;
    std::vector<std::unordered_map<Token, bool>> scopes_;
    std::vector<std::unordered_set<Token>> usage_;
    FunctionType currentFunction_ = FunctionType::NONE;

    std::vector<std::unordered_map<Token, std::size_t>> localLocations_;
    std::unordered_map<Token, std::size_t> globalLocations_;

    std::vector<std::size_t> localIndexStack_;
    std::size_t globalIndex_ = 0;

    void beginScope();
    void endScope();

    void resolve(Expression& e);
    void resolve(Statement& s);

    void declare(const Token& name);
    void define(const Token& name);
    void resolveLocal(Expression* expr, const Token& name);

    void resolveFunction(Function& f, FunctionType type);
    void defineGlobal(const Token& name);
    void defineLocal(const Token& name);
};


#endif //LOX_RESOLVER_H
