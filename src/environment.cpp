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

    throw RuntimeError(token,
                       "Undefined variable '" + token.getLexeme() + "'.");
}
