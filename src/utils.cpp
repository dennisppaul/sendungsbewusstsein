#include "utils.hpp"

#include <iomanip>
#include <iostream>

// TODO add `console` and `error` for output

void Utils::print_byte(unsigned char byte) {
//    for (int i = 0; i < 8; ++i) {
    for (int i = 7; i >= 0; --i) {
        unsigned char mask = 1 << (7 - i);
        std::cout << ((byte & mask) ? "1" : "0");
    }
}

void Utils::print_byte_array_as_bits(const SimpleBLE::ByteArray &bytes) {
    for (auto b: bytes) {
        print_byte(b);
        std::cout << " ";
    }
    std::cout << std::endl;
}

void Utils::print_byte_array_as_dec(const SimpleBLE::ByteArray &bytes) {
    std::cout << std::dec;
    for (auto b: bytes) {
        std::cout << std::setw(3) << std::setfill('0') << (uint32_t) ((uint8_t) b) << "  ";
//        std::cout << static_cast<int>(b) << " ";
    }
    std::cout << std::endl;
}

void Utils::print_byte_array_as_hex(const SimpleBLE::ByteArray &bytes) {
    for (auto b: bytes) {
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << (uint32_t) ((uint8_t) b) << " ";
    }
    std::cout << std::endl;
    std::cout << std::dec;
}

std::optional<std::size_t> Utils::getUserInputInt(const std::string &line, std::size_t max) {
    std::size_t ret;

    while (!std::cin.eof()) {
        std::cout << line << " (0-" << max << "): ";
        std::cin >> ret;

        if (!std::cin) {
            return {};
        }

        if (ret <= max) {
            return ret;
        }
    }
    return {};
}

std::optional<SimpleBLE::Adapter> Utils::getAdapter() {
    if (!SimpleBLE::Adapter::bluetooth_enabled()) {
        std::cout << "Bluetooth is not enabled!" << std::endl;
        return {};
    }

    auto adapter_list = SimpleBLE::Adapter::get_adapters();

    // no adapter found
    if (adapter_list.empty()) {
        std::cerr << "No adapter was found." << std::endl;
        return {};
    }

    // only one found, returning directly
    if (adapter_list.size() == 1) {
        auto adapter = adapter_list.at(0);
//        std::cout << "Using adapter: " << adapter.identifier() << " [" << adapter.address() << "]" << std::endl;
        return adapter;
    }

    // multiple adapters found, ask user to select one
    std::cout << "Available adapters:" << std::endl;
    int       i = 0;
    for (auto &adapter: adapter_list) {
        std::cout << "[" << i++ << "] " << adapter.identifier() << " [" << adapter.address() << "]" << std::endl;
    }

    auto adapter_selection = Utils::getUserInputInt("Select an adapter", adapter_list.size() - 1);

    if (!adapter_selection.has_value()) {
        return {};
    }

    return adapter_list[adapter_selection.value()];
}
