//
// Created by chrku on 22.11.2021.
//

#include "loxclass.h"

#include <utility>

LoxClass::LoxClass(std::string  name)
    : name_{std::move(name)}
{

}

const std::string& LoxClass::getName() const {
    return name_;
}
