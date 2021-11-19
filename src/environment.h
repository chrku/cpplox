//
// Created by chrku on 14.08.2021.
//

#ifndef LOX_ENVIRONMENT_H
#define LOX_ENVIRONMENT_H

#include "types.h"

#include <string>
#include <iostream>

/*!
 * This represents an environment in lox and it associates
 * variable names with values
 */
class Environment {
public:
    std::size_t define();

    /*!
     * Define new variable
     * @param value value of the variable
     * @return index of that variable in array
     */
    std::size_t define(LoxType value);

    /*!
     * Get value for variable
     * @param token token representing variable name
     * @return value if available
     * @throws RuntimeError if it does not exist
     */
    const LoxType& get(std::size_t index);

    const LoxType& getAt(std::size_t index, int distance);

    /*!
     * Re-assign variable
     * @param token token representing variable name
     * @param value new value
     * @throws RuntimeError if it does not exist
     */
    void assign(std::size_t index, LoxType value);

    void assignAt(std::size_t index, LoxType value, int distance);

    /*!
     * Set enclosing scope
     * @param enclosing handle to enclosing scope
     */
    void setEnclosing(std::shared_ptr<Environment> enclosing);

    Environment* ancestor(int distance);
private:
    std::shared_ptr<Environment> enclosing_; // Represents upper-level scope
    std::vector<LoxType> values_;
};


#endif //LOX_ENVIRONMENT_H
