//
// Created by chrku on 07.08.2021.
//

#include "string_utils.h"

#include <string>

std::vector<std::string> tokenize(std::string_view str, std::string_view delimiters) {
    using size_type = std::string_view::size_type;

    size_type pos;
    size_type last_pos = 0;
    size_type length = str.length();
    std::vector<std::string> results;

    while (last_pos < length + 1) {
        // Find first pos of delimiters
        pos = str.find_first_of(delimiters, last_pos);
        // If delimiters not found, return
        if (pos == std::string::npos) {
            pos = length;
        }

        if (pos != last_pos) {
            results.emplace_back(str.substr(last_pos, pos - last_pos));
        }

        last_pos = pos + 1;
    }

    return results;
}
