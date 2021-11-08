//
// Created by chrku on 14.08.2021.
//

#ifndef LOX_ENVIRONMENT_H
#define LOX_ENVIRONMENT_H

#include "types.h"

#include <unordered_map>
#include <string>
#include <iostream>

/*!
 * This represents an environment in lox and it associates
 * variable names with values
 */
class Environment {
public:
    /*!
     * Define new variable
     * @param name name of the variable
     * @param value value of the variable
     */
    void define(const std::string& name, LoxType value);

    /*!
     * Get value for variable
     * @param token token representing variable name
     * @return value if available
     * @throws RuntimeError if it does not exist
     */
    const LoxType& get(const Token& token);

    const LoxType& getAt(const Token& token, int distance);

    /*!
     * Re-assign variable
     * @param token token representing variable name
     * @param value new value
     * @throws RuntimeError if it does not exist
     */
    void assign(const Token& name, LoxType value);

    void assignAt(const Token& name, LoxType value, int distance);

    /*!
     * Set enclosing scope
     * @param enclosing handle to enclosing scope
     */
    void setEnclosing(std::shared_ptr<Environment> enclosing);

    Environment* ancestor(int distance);
private:
    std::shared_ptr<Environment> enclosing_; // Represents upper-level scope
    std::unordered_map<std::string, LoxType> values_;
};


#endif //LOX_ENVIRONMENT_H
