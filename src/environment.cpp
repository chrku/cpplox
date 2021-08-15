//
// Created by chrku on 14.08.2021.
//

#include "environment.h"

#include "interpreter.h"

#include <utility>

void Environment::define(const std::string& name, LoxType value) {
    values_[name] = std::move(value);
}

const LoxType& Environment::get(const Token& token) {
    if (values_.count(token.getLexeme())) {
        return values_[token.getLexeme()];
    }

    if (enclosing_) { return enclosing_->get(token); }

    throw RuntimeError(token,
                       "Undefined variable '" + token.getLexeme() + "'.");
}

void Environment::assign(const Token& name, LoxType value) {
    if (values_.count(name.getLexeme())) {
        values_[name.getLexeme()] = std::move(value);
        return;
    }

    if (enclosing_) {
        enclosing_->assign(name, value);
        return;
    }

    throw RuntimeError(name,
                       "Undefined variable '" + name.getLexeme() + "'.");
}

void Environment::setEnclosing(std::shared_ptr<Environment> enclosing) {
    enclosing_ = std::move(enclosing);
}
