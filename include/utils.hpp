#pragma once

#include <optional>
#include "simpleble/SimpleBLE.h"

namespace Utils {
    void print_byte(unsigned char byte);

    void print_byte_array_as_bits(const SimpleBLE::ByteArray &bytes);

    void print_byte_array_as_dec(const SimpleBLE::ByteArray &bytes);

    void print_byte_array_as_hex(const SimpleBLE::ByteArray &bytes);

    std::optional<SimpleBLE::Adapter> getAdapter();

    std::optional<std::size_t> getUserInputInt(const std::string &line, std::size_t max);
}  // namespace Utils
