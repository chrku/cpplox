//
// Created by Christoph Kummer on 25.11.21.
//

#ifndef LOX_LOXINSTANCE_H
#define LOX_LOXINSTANCE_H

#include "loxclass.h"

class LoxInstance {
public:
    explicit LoxInstance(std::shared_ptr<LoxClass> klass);

    [[nodiscard]] const std::shared_ptr<LoxClass> &getClass() const;
private:
    std::shared_ptr<LoxClass> class_;
};


#endif //LOX_LOXINSTANCE_H
