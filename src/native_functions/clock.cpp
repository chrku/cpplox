//
// Created by chrku on 31.10.2021.
//

#include "clock.h"

#include <chrono>

Clock::Clock(bool test_mode) : testMode_(test_mode) {

}

LoxType Clock::call(Interpreter &interpreter, std::vector<LoxType> &arguments) {
    if (testMode_) { return 0.0; }
    double now = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count());
    return now;
}

int Clock::arity() {
    return 0;
}


Clock::~Clock() = default;
