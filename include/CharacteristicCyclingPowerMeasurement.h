#pragma once

#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"

class CharacteristicCyclingPowerMeasurement : public CharacteristicAbstract {
public:
    CharacteristicCyclingPowerMeasurement(Peripheral *peripheral,
                                          int connected_device_index,
                                          int supported_characteristic_index)
            : CharacteristicAbstract(peripheral,
                                     connected_device_index,
                                     supported_characteristic_index) {}

    void init() override {
        subscribe();
    }

    void subscribe() override {
        console
                << "subscribe to service "
                << SERVICE_CYCLING_POWER
                << " with characteristic "
                << CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N
                << std::endl;
        console
                << "TODO implement"
                << std::endl;
        // TODO implement
    }

    void unsubscribe() override {}

    void read() override {}

    void write() override {}

    const char *name() override { return fName; }

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE_CYCLING_POWER,
                                                       CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N,
                                                       [](
                                                               Peripheral *peripheral,
                                                               int connected_device_index,
                                                               int supported_characteristic_index) -> std::unique_ptr<CharacteristicAbstract> {
                                                           return std::make_unique<CharacteristicCyclingPowerMeasurement>(
                                                                   peripheral,
                                                                   connected_device_index,
                                                                   supported_characteristic_index);
                                                       });
    }

private:
    constexpr static const char *fName = "cycling_power_measurement";
};
