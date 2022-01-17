//
// Created by chrku on 22.11.2021.
//

#include "loxclass.h"
#include "loxinstance.h"
#include "loxfunction.h"

#include <utility>

LoxClass::LoxClass(std::string name, std::unordered_map<Token, std::shared_ptr<LoxFunction>> methods)
    : name_{std::move(name)}, methods_(std::move(methods))
{
}

const std::string& LoxClass::getName() const {
    return name_;
}

LoxType LoxClass::call(Interpreter& interpreter, std::vector<LoxType>& arguments) {
    std::shared_ptr<LoxInstance> new_instance = std::make_shared<LoxInstance>(shared_from_this());
    auto initializer = getMethod(Token(TokenType::IDENTIFIER, "init", 0));
    if (initializer) {
        initializer->bind(new_instance)->call(interpreter, arguments);
    }
    return new_instance;
}

int LoxClass::arity() {
    auto initializer = getMethod(Token(TokenType::IDENTIFIER, "init", 0));
    if (initializer) { return initializer->arity(); }
    else { return 0; }
}

std::shared_ptr<LoxFunction> LoxClass::getMethod(const Token& name) {
    if (methods_.count(name)) {
        return methods_.at(name);
    }

    return {};
}
