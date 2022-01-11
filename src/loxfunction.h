//
// Created by chrku on 01.11.2021.
//

#ifndef LOX_LOXFUNCTION_H
#define LOX_LOXFUNCTION_H

#include "types.h"
#include "statements.h"
#include "token.h"
#include "environment.h"

/**
 * This represents user-defined functions in Lox
 */
class LoxFunction : public Callable {
public:
    /**
     * Constructor used for normal functions
     * @param function Reference to normal function in AST
     * @param closure enclosing environment used for lookups
     */
    LoxFunction(Function& function, std::shared_ptr<Environment> closure);

    /**
     * Constructor used for anonymous functions
     * @param function Reference to anonymous function
     * @param closure enclosing environment used for lookups
     */
    LoxFunction(FunctionExpression& function, std::shared_ptr<Environment> closure);

    LoxFunction(std::vector<std::shared_ptr<Statement>>  statements_,
                std::vector<Token>  params_,
                std::shared_ptr<Environment> closure_);

    std::shared_ptr<LoxFunction> bind(const std::shared_ptr<LoxInstance>& instance);

    LoxType call(Interpreter &interpreter, std::vector<LoxType> &arguments) override;

    int arity() override;

    ~LoxFunction() override = default;


private:
    std::vector<std::shared_ptr<Statement>> statements_;
    std::vector<Token> params_;
    std::shared_ptr<Environment> closure_;
};


#endif //LOX_LOXFUNCTION_H
