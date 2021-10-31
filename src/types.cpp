//
// Created by chrku on 14.08.2021.
//

#include "types.h"

std::string stringify(const LoxType& l) {
    std::string return_value;

    std::visit(overload{
            [&](double d) {
                return_value = std::to_string(d);
            },
            [&](const std::string &s) {
                return_value = s;
            },
            [&](const NullType &n) {
                return_value = "nil";
            },
            [&](bool b) {
                return_value = std::to_string(b);
            },
            [&](const std::shared_ptr<Callable>& c) {
                return_value = std::to_string(reinterpret_cast<std::uintptr_t>(c.get()));
            }
    }, l);

    return return_value;
}
