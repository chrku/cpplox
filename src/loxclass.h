//
// Created by chrku on 22.11.2021.
//

#ifndef LOX_LOXCLASS_H
#define LOX_LOXCLASS_H

#include <string>

class LoxClass {
public:
    LoxClass(std::string  name);

    const std::string& getName() const;
private:
    std::string name_;
};


#endif //LOX_LOXCLASS_H
