//
// Created by chrku on 07.08.2021.
//

#ifndef LOX_TOKEN_H
#define LOX_TOKEN_H

#include "token_type.h"

/*!
 * Class representing tokens
 */
class Token {
public:

private:
    const TokenType type_;
    const std::string lexeme_;
    const int line_;
};


#endif //LOX_TOKEN_H
