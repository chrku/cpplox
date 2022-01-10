//
// Created by Christoph Kummer on 25.11.21.
//

#include "loxinstance.h"
#include "loxfunction.h"
#include "interpreter.h"

#include <utility>

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass) : class_{std::move(klass)}
{}

const std::shared_ptr<LoxClass>& LoxInstance::getClass() const {
    return class_;
}

LoxType LoxInstance::get(const Token& name) {
    if (fields_.count(name)) {
        return fields_.at(name);
    }

    if (class_->getMethod(name)) {
        return std::static_pointer_cast<Callable>(class_->getMethod(name));
    }

    throw RuntimeError(name,
                       "Undefined property '" + name.getLexeme() + "'.");
}

void LoxInstance::set(const Token& name, LoxType value) {
    fields_[name] = std::move(value);
}
