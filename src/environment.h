//
// Created by chrku on 14.08.2021.
//

#ifndef LOX_ENVIRONMENT_H
#define LOX_ENVIRONMENT_H

#include "types.h"

#include <unordered_map>
#include <string>
#include <iostream>

class Environment {
public:
    void define(const std::string& name, LoxType value);
    const LoxType& get(const Token& token);
    void assign(const Token& name, LoxType value);

    void setEnclosing(std::shared_ptr<Environment> enclosing);
private:
    std::shared_ptr<Environment> enclosing_;
    std::unordered_map<std::string, LoxType> values_;
};


#endif //LOX_ENVIRONMENT_H
