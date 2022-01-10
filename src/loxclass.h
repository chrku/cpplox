//
// Created by chrku on 22.11.2021.
//

#ifndef LOX_LOXCLASS_H
#define LOX_LOXCLASS_H

#include <string>
#include <unordered_map>

#include "types.h"

class LoxFunction;

class LoxClass : public Callable, public std::enable_shared_from_this<LoxClass> {
public:
    LoxClass(std::string name,
             std::unordered_map<Token, std::shared_ptr<LoxFunction>>);

    const std::string& getName() const;

    std::shared_ptr<LoxFunction> getMethod(const Token& name);

    LoxType call(Interpreter& interpreter, std::vector<LoxType>& arguments) override;

    int arity() override;

    ~LoxClass() override = default;

private:
    std::string name_;
    std::unordered_map<Token, std::shared_ptr<LoxFunction>> methods_;
};


#endif //LOX_LOXCLASS_H
