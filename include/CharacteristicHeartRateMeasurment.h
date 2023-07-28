#pragma once

#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"

class CharacteristicHeartRateMeasurment : public CharacteristicAbstract {
public:

    void subscribe() override {
        std::cout
                << "subscribe to service "
                << SERVICE_HEART_RATE
                << " with characteristic "
                << CHARACTERISTIC_HEART_RATE_MEASUREMENT_N
                << std::endl;
    }

    void unsubscribe() override {}

    void read() override {}

    void write() override {}

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE_HEART_RATE,
                                                       CHARACTERISTIC_HEART_RATE_MEASUREMENT_N,
                                                       []() -> std::unique_ptr<CharacteristicAbstract> { return std::make_unique<CharacteristicHeartRateMeasurment>(); });
    }
};
