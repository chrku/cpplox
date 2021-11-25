//
// Created by Christoph Kummer on 25.11.21.
//

#include "loxinstance.h"
#include "interpreter.h"

#include <utility>

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass) : class_{std::move(klass)}
{}

const std::shared_ptr<LoxClass>& LoxInstance::getClass() const {
    return class_;
}

LoxType LoxInstance::get(const Token& name) {
    if (fields_.count(name)) {
        return fields_[name];
    }

    throw RuntimeError(name,
                       "Undefined property '" + name.getLexeme() + "'.");
}
