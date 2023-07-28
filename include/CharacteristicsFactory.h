#pragma once

#include <iostream>
#include <map>
#include <string>
#include <memory>

#include "CharacteristicAbstract.h"

class CharacteristicFactory {
public:
    using createFn = std::unique_ptr<CharacteristicAbstract> (*)();

    static void register_characteristic(const std::string& service, const std::string& characteristic, createFn createFn) {
        characteristic_registry()[std::make_pair(service, characteristic)] = createFn;
    }

    static std::unique_ptr<CharacteristicAbstract> create(const std::string& service, const std::string& characteristic) {
        auto it = characteristic_registry().find(std::make_pair(service, characteristic));
        if (it != characteristic_registry().end()) {
            return it->second();
        }
        return nullptr;
    }

private:
    static std::map<std::pair<std::string, std::string>, createFn>& characteristic_registry() {
        static std::map<std::pair<std::string, std::string>, createFn> registry;
        return registry;
    }
};
