#pragma once

#include <iostream>
#include <sstream>

template<typename OutputStream = std::ostream>
class Console {
public:
    template<typename T>
    Console &operator<<(const T &value) {
        if (output_stream && output_to_std_out) {
            buffer << value;
        }
        return *this;
    }

    Console &operator<<(std::ostream &(*manipulator)(std::ostream &)) {
        if (manipulator == static_cast<std::ostream &(*)(std::ostream &)>(std::endl)) {
            if (output_stream && output_to_std_out) {
                print(buffer.str() + std::string("\n\r"));
            }
        }
        return *this;
    }

    void enable_output(bool is_enabled) {
        output_to_std_out = is_enabled;
    }

    void set_output_stream(OutputStream &os) {
        output_stream = &os;
    }

    void set_prefix(const std::string &prefix) {
        PREFIX = prefix;
    }

    Console &flush(bool newline = true) {
        if (output_stream && output_to_std_out) {
            print(buffer.str() + (newline ? std::string("\n\r") : std::string("")));
        }
        return *this;
    }

    Console &append() {
        if (output_stream && output_to_std_out) {
            (*output_stream) << buffer.str();
            buffer.str("");
            buffer.clear();
        }
        return *this;
    }

    Console &newline() {
        if (output_stream && output_to_std_out) {
            (*output_stream) << "\n\r";
        }
        return *this;
    }

private:
//    static const constexpr char *PREFIX           = "+++ ";
    std::ostringstream buffer;
    std::string        PREFIX            = "+++ ";
    bool               output_to_std_out = true;
    OutputStream       *output_stream    = nullptr;

    void print(const std::string &output) {
        if (output_stream) {
            (*output_stream) << PREFIX << output;
            buffer.str("");
            buffer.clear();
        }
    }
};

extern Console<std::ostream> console;
extern Console<std::ostream> error;
extern Console<std::ostream> logger;
