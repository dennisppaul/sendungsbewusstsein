#pragma once

#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"

class CharacteristicCyclingPowerMeasurement : public CharacteristicAbstract {
public:

    void subscribe() override {
        std::cout
                << "subscribe to service "
                << SERVICE_CYCLING_POWER
                << " with characteristic "
                << CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N
                << std::endl;
    }

    void unsubscribe() override {}

    void read() override {}

    void write() override {}

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE_CYCLING_POWER,
                                                       CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N,
                                                       []() -> std::unique_ptr<CharacteristicAbstract> { return std::make_unique<CharacteristicCyclingPowerMeasurement>(); });
    }
};
