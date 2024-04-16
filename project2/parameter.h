#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>

class Parameter {
public:
    std::string value;

    std::string toString() const {
        return value;
    }
};

#endif