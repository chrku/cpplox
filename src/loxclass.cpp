//
// Created by chrku on 22.11.2021.
//

#include "loxclass.h"
#include "loxinstance.h"

#include <utility>

LoxClass::LoxClass(std::string  name)
    : name_{std::move(name)}
{
}

const std::string& LoxClass::getName() const {
    return name_;
}

LoxType LoxClass::call(Interpreter& interpreter, std::vector<LoxType>& arguments) {
    std::shared_ptr<LoxInstance> new_instance = std::make_shared<LoxInstance>(shared_from_this());
    return new_instance;
}

int LoxClass::arity() {
    return 0;
}
