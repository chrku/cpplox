//
// Created by chrku on 07.08.2021.
//

#ifndef LOX_STRING_UTILS_H
#define LOX_STRING_UTILS_H

#include <string_view>
#include <vector>

/*!
 * Tokenize string, using given delimiters
 * @param str input string view
 * @param delimiters delimiters to use for tokenization
 * @return vector of tokens
 */
std::vector<std::string> tokenize(std::string_view str,
                                  std::string_view delimiters);

#endif //LOX_STRING_UTILS_H
