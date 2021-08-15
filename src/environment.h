//
// Created by chrku on 14.08.2021.
//

#ifndef LOX_ENVIRONMENT_H
#define LOX_ENVIRONMENT_H

#include "types.h"

#include <unordered_map>
#include <string>

class Environment {
public:
    void define(const std::string& name, LoxType value);
    const LoxType& get(const Token& token);
    void assign(const Token& name, LoxType value);
private:
    std::unordered_map<std::string, LoxType> values_;
};


#endif //LOX_ENVIRONMENT_H
