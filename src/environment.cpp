//
// Created by chrku on 14.08.2021.
//

#include "environment.h"

#include "interpreter.h"

std::size_t Environment::define() {
    values_.emplace_back(NullType{});
    return values_.size() - 1;
}

std::size_t Environment::define(LoxType value) {
    values_.emplace_back(std::move(value));
    return values_.size() - 1;
}

const LoxType& Environment::get(std::size_t index) {
    if (index < values_.size()) {
        return values_[index];
    }

    if (enclosing_) { return enclosing_->get(index); }

    throw std::runtime_error("This should never happen.");
}

const LoxType& Environment::getAt(std::size_t index, int distance) {
    return ancestor(distance)->get(index);
}

void Environment::assign(std::size_t index, LoxType value) {
    if (index < values_.size()) {
        values_[index] = std::move(value);
        return;
    }

    if (enclosing_) {
        enclosing_->assign(index, value);
        return;
    }

    throw std::runtime_error("This should never happen.");
}

void Environment::assignAt(std::size_t index, LoxType value, int distance) {
    ancestor(distance)->assign(index, std::move(value));
}

void Environment::setEnclosing(std::shared_ptr<Environment> enclosing) {
    enclosing_ = std::move(enclosing);
}

Environment* Environment::ancestor(int distance) {
    Environment* environment = this;
    for (int i = 0; i < distance; ++i) {
        environment = environment->enclosing_.get();
    }

    return environment;
}



