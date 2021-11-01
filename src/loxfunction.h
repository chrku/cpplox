//
// Created by chrku on 01.11.2021.
//

#ifndef LOX_LOXFUNCTION_H
#define LOX_LOXFUNCTION_H

#include "types.h"
#include "statements.h"
#include "token.h"
#include "environment.h"

class LoxFunction : public Callable {
public:
    LoxFunction(const Function& function, std::shared_ptr<Environment> closure);

    LoxFunction(const FunctionExpression& function, std::shared_ptr<Environment> closure);

    LoxType call(Interpreter &interpreter, std::vector<LoxType> &arguments) override;

    int arity() override;

    ~LoxFunction() override = default;
private:
    std::vector<std::shared_ptr<Statement>> statements_;
    std::vector<Token> params_;
    std::shared_ptr<Environment> closure_;
};


#endif //LOX_LOXFUNCTION_H
