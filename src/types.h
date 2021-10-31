//
// Created by chrku on 08.08.2021.
//

#ifndef LOX_TYPES_H
#define LOX_TYPES_H

#include "token.h"

#include "utils.h"

#include <memory>
#include <vector>
#include <utility>

/*!
 * Represents null values
 */
class NullType {};
class Callable;
class Interpreter;

/*!
 * Representation of the types a Lox expression can return
 */
using LoxType = std::variant<double, std::string, bool, NullType, std::shared_ptr<Callable>>;

class Callable {
public:
    virtual LoxType call(Interpreter& interpreter, std::vector<LoxType>& arguments) = 0;
    virtual int arity() = 0;
    virtual ~Callable() = default;
};

std::string stringify(const LoxType &l);


#endif //LOX_TYPES_H
