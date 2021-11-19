//
// Created by chrku on 14.08.2021.
//

#ifndef LOX_ENVIRONMENT_H
#define LOX_ENVIRONMENT_H

#include "types.h"

#include <string>
#include <iostream>

/*!
 * This represents an environment in Lox,
 * which stores values of variables
 */
class Environment {
public:
    /**
     * Define new null-initialized variable
     * @return variable index for later lookup
     */
    std::size_t define();

    /*!
     * Define new variable, initialized with value
     * @param value value of the variable
     * @return variable index for later lookup
     */
    std::size_t define(LoxType value);

    /*!
     * Get value for variable
     * @param index index of variable in array
     * @return value if available
     */
    const LoxType& get(std::size_t index);

    /**
     * Get variable at specific depth
     * @param index index of variable in array
     * @param distance distance from current environment
     * @return reference to value of variable
     */
    const LoxType& getAt(std::size_t index, int distance);

    /*!
     * Re-assign variable
     * @param index index of variable in array
     * @param value new value
     */
    void assign(std::size_t index, LoxType value);

    /**
     * Re-assign variable value
     * @param index index of variable in array
     * @param value new value
     * @param distance distance from current environment
     */
    void assignAt(std::size_t index, LoxType value, int distance);

    /*!
     * Set enclosing scope
     * @param enclosing handle to enclosing scope
     */
    void setEnclosing(std::shared_ptr<Environment> enclosing);

    /**
     * Get ancestor of environment, i.e. enclosing scope
     * @param distance distance of scope from current environment
     * @return reference to environment
     */
    Environment* ancestor(int distance);
private:
    std::shared_ptr<Environment> enclosing_; // Represents upper-level scope
    std::vector<LoxType> values_; // Represents value array
};


#endif //LOX_ENVIRONMENT_H
