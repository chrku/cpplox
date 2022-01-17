//
// Created by chrku on 01.11.2021.
//

#include "loxfunction.h"

#include <utility>
#include "interpreter.h"


LoxFunction::LoxFunction(Function& function, std::shared_ptr<Environment> closure, bool is_init)
    : statements_(function.getBody()), params_(function.getParams()), closure_(std::move(closure)),
      isInit_(is_init)
{}


LoxFunction::LoxFunction(FunctionExpression &function, std::shared_ptr<Environment> closure, bool is_init)
    : statements_(function.getBody()), params_(function.getParams()), closure_(std::move(closure)),
      isInit_(is_init)
{}

LoxFunction::LoxFunction(std::vector<std::shared_ptr<Statement>> statements,
                         std::vector<Token> params,
                         std::shared_ptr<Environment> closure,
                         bool is_init)
     : statements_(std::move(statements)), params_(std::move(params)), closure_(std::move(closure)),
       isInit_(is_init)
{}

std::shared_ptr<LoxFunction> LoxFunction::bind(const std::shared_ptr<LoxInstance>& instance) {
    std::shared_ptr<Environment> env = std::make_shared<Environment>();
    env->setEnclosing(closure_);
    env->define(instance);
    return std::make_shared<LoxFunction>(statements_, params_, env, isInit_);
}

LoxType LoxFunction::call(Interpreter& interpreter, std::vector<LoxType>& arguments) {
    std::shared_ptr<Environment> environment = std::make_shared<Environment>();
    environment->setEnclosing(closure_);

    for (int i = 0; i < params_.size(); ++i) {
        environment->define(arguments[i]);
    }

    try {
        interpreter.executeBlock(statements_, environment);
    } catch (const ReturnException& r) {
        if (isInit_) { return closure_->getAt(0, 0); }
        return r.getValue();
    }

    if (isInit_) { return closure_->getAt(0, 0); }
    return NullType{};
}

int LoxFunction::arity() {
    return static_cast<int>(params_.size());
}

