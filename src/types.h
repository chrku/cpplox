//
// Created by chrku on 08.08.2021.
//

#ifndef LOX_TYPES_H
#define LOX_TYPES_H

#include "token.h"

#include "utils.h"

#include <memory>
#include <utility>

/*!
 * Represents null values
 */
class NullType {};

/*!
 * Representation of the types a Lox expression can return
 */
using LoxType = std::variant<double, std::string, bool, NullType>;

std::string stringify(const LoxType &l);


#endif //LOX_TYPES_H
