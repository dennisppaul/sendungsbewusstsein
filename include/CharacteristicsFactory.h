#pragma once

#include <iostream>
#include <map>
#include <string>
#include <memory>

#include "simpleble/SimpleBLE.h"
#include "CharacteristicAbstract.h"

class CharacteristicFactory {
public:
    using createFn = std::unique_ptr<CharacteristicAbstract> (*)(SimpleBLE::Peripheral *, int, int);

    static void
    register_characteristic(const std::string &service, const std::string &characteristic, createFn createFn) {
        characteristic_registry()[std::make_pair(service, characteristic)] = createFn;
    }

    static std::unique_ptr<CharacteristicAbstract>
    create(const std::string &service,
           const std::string &characteristic,
           SimpleBLE::Peripheral *peripheral,
           const int connected_device_index,
           const int supported_characteristic_index) {
        auto it = characteristic_registry().find(std::make_pair(service, characteristic));
        if (it != characteristic_registry().end()) {
            return it->second(peripheral, connected_device_index, supported_characteristic_index);
        }
        return nullptr;
    }

private:
    static std::map<std::pair<std::string, std::string>, createFn> &characteristic_registry() {
        static std::map<std::pair<std::string, std::string>, createFn> registry;
        return registry;
    }
};
