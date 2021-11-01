//
// Created by chrku on 01.11.2021.
//

#include "loxfunction.h"
#include "environment.h"
#include "interpreter.h"

#include <utility>

LoxFunction::LoxFunction(const Function& function)
    : statements_(function.getBody()), params_(function.getParams())
{}

LoxType LoxFunction::call(Interpreter& interpreter, std::vector<LoxType>& arguments) {
    std::shared_ptr<Environment> environment = std::make_shared<Environment>();
    environment->setEnclosing(interpreter.getEnvironment());

    for (int i = 0; i < params_.size(); ++i) {
        environment->define(params_[i].getLexeme(), arguments[i]);
    }

    interpreter.executeBlock(statements_, environment);

    return NullType{};
}

int LoxFunction::arity() {
    return static_cast<int>(params_.size());
}

