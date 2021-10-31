//
// Created by chrku on 31.10.2021.
//

#include "clock.h"

#include <chrono>

LoxType Clock::call(Interpreter &interpreter, std::vector<LoxType> &arguments) {
    double now = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count());
    return now;
}

int Clock::arity() {
    return 0;
}

Clock::~Clock() = default;
