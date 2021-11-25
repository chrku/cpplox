//
// Created by Christoph Kummer on 25.11.21.
//

#ifndef LOX_LOXINSTANCE_H
#define LOX_LOXINSTANCE_H

#include <unordered_map>

#include "loxclass.h"

class LoxInstance {
public:
    explicit LoxInstance(std::shared_ptr<LoxClass> klass);

    [[nodiscard]] const std::shared_ptr<LoxClass>& getClass() const;

    LoxType get(const Token& name);

    void set(const Token& name, LoxType value);
private:
    std::shared_ptr<LoxClass> class_;

    std::unordered_map<Token, LoxType> fields_;
};


#endif //LOX_LOXINSTANCE_H
