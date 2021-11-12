//
// Created by chrku on 31.10.2021.
//

#ifndef LOX_CLOCK_H
#define LOX_CLOCK_H

#include "types.h"

class Clock : public Callable {
public:
    Clock(bool test_mode);

    LoxType call(Interpreter &interpreter, std::vector<LoxType> &arguments) override;

    int arity() override;

    ~Clock() override;
private:
    bool testMode_;
};


#endif //LOX_CLOCK_H
