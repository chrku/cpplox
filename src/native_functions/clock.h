//
// Created by chrku on 31.10.2021.
//

#ifndef LOX_CLOCK_H
#define LOX_CLOCK_H

#include "types.h"

/**
 * Native function - returns time since Unix epoch
 */
class Clock : public Callable {
public:
    /**
     * Constructor - executed once at interpreter startup
     * @param test_mode enables test mode, gives deterministic output for testing
     */
    explicit Clock(bool test_mode);

    LoxType call(Interpreter &interpreter, std::vector<LoxType> &arguments) override;

    int arity() override;

    ~Clock() override;
private:
    bool testMode_;
};


#endif //LOX_CLOCK_H
