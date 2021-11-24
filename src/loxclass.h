//
// Created by chrku on 22.11.2021.
//

#ifndef LOX_LOXCLASS_H
#define LOX_LOXCLASS_H

#include <string>

#include "types.h"

class LoxClass : public Callable, public std::enable_shared_from_this<LoxClass> {
public:
    LoxClass(std::string name);

    const std::string& getName() const;

    LoxType call(Interpreter& interpreter, std::vector<LoxType>& arguments) override;

    int arity() override;

    ~LoxClass() override = default;

private:
    std::string name_;
};


#endif //LOX_LOXCLASS_H
