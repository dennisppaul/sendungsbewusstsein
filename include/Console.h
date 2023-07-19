#pragma once

#include <iostream>
#include <sstream>

class Console {
public:
    template<typename T>
    Console &operator<<(const T &value) {
        if (output_to_std_out) {
            buffer << value;
        }
        return *this;
    }

    Console &operator<<(std::ostream &(*manipulator)(std::ostream &)) {
        if (manipulator == static_cast<std::ostream &(*)(std::ostream &)>(std::endl)) {
            if (output_to_std_out) {
                print(buffer.str() + std::string("\n\r"));
            }
        }
        return *this;
    }

    void enable_output(bool is_enabled) {
        output_to_std_out = is_enabled;
    }

    Console &flush() {
        if (output_to_std_out) {
            print(buffer.str() + std::string("\n\r"));
        }
        return *this;
    }

private:
    std::ostringstream          buffer;
    static const constexpr char *PREFIX           = "+++ ";
    bool                        output_to_std_out = true;


    void print(const std::string &output) {
        std::cout << PREFIX << output;
        buffer.str("");
        buffer.clear();
    }
};

extern Console console;
