//
// Created by Christoph Kummer on 25.11.21.
//

#include "loxinstance.h"

#include <utility>

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass) : class_{std::move(klass)}
{}

const std::shared_ptr<LoxClass>& LoxInstance::getClass() const {
    return class_;
}
