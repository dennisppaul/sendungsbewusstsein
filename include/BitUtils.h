#pragma once

#include <iostream>
#include <cstdint>

bool is_bit_set(uint16_t value, uint8_t bit_position) {
    return (value & (1 << bit_position)) != 0;
}

uint16_t bytes_to_uint16(uint8_t high, uint8_t low) {
    /* big endian */
    return (static_cast<uint16_t>(high) << 8) | low;
}

int16_t bytes_to_int16(uint8_t high, uint8_t low) {
    /* big endian */
    byte b;
    return (high << 8) | low;
}

uint32_t bytes_to_uint32(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0) {
    return (uint32_t) ((b3 << 24) | (b2 << 16) | (b1 << 8) | b0);
}

void test_bit_utils() {
    uint8_t  high          = 0b10001100;
    uint8_t  low           = 0b11000010;
    uint16_t combinedValue = bytes_to_uint16(high, low);

    /* test from lowest to highest bit */
    for (int i = 0; i < 16; ++i) {
        std::cout << i << " > " << is_bit_set(combinedValue, i) << std::endl;
    }
}
