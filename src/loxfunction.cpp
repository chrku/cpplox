//
// Created by chrku on 01.11.2021.
//

#include "loxfunction.h"
#include "interpreter.h"


LoxFunction::LoxFunction(Function& function, std::shared_ptr<Environment> closure)
    : statements_(function.getBody()), params_(function.getParams()), closure_(std::move(closure))
{}


LoxFunction::LoxFunction(FunctionExpression &function, std::shared_ptr<Environment> closure)
    : statements_(function.getBody()), params_(function.getParams()), closure_(std::move(closure))
{}

LoxType LoxFunction::call(Interpreter& interpreter, std::vector<LoxType>& arguments) {
    std::shared_ptr<Environment> environment = std::make_shared<Environment>();
    environment->setEnclosing(closure_);

    for (int i = 0; i < params_.size(); ++i) {
        environment->define(arguments[i]);
    }

    try {
        interpreter.executeBlock(statements_, environment);
    } catch (const ReturnException& r) {
        return r.getValue();
    }

    return NullType{};
}

int LoxFunction::arity() {
    return static_cast<int>(params_.size());
}

