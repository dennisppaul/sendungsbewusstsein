#pragma once

#include "Sendungsbewusstsein.h"

class Feature {
public:
    Feature(std::string name, std::function<void(float)> set_value = nullptr) :
            fName(name),
            fSetValueFunction(set_value) {}

    void invoke_set_value(float value) {
        if (fSetValueFunction) {
            fSetValueFunction(value);
        } else {
            error
                    << "feature "
                    << "'"
                    << name()
                    << "'"
                    << " cannot set values"
                    << endl;
        }
    }

    std::string &name() {
        return fName;
    }

private:
    std::string                fName;
    std::function<void(float)> fSetValueFunction;
};