#pragma once

#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"

class CharacteristicHeartRateMeasurment : public CharacteristicAbstract {
public:
    CharacteristicHeartRateMeasurment(Peripheral *peripheral,
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
                << "subscribe to '"
                << name()
                << "' "
                << "as service "
                << SERVICE_HEART_RATE
                << " with characteristic "
                << CHARACTERISTIC_HEART_RATE_MEASUREMENT_N
                << std::endl;

        auto mHeartRateCallback = bind( // NOLINT(*-avoid-bind)
                &CharacteristicHeartRateMeasurment::notify,
                this,
                std::placeholders::_1);
        fPeripheral->notify(SERVICE_HEART_RATE,
                            CHARACTERISTIC_HEART_RATE_MEASUREMENT_N,
                            mHeartRateCallback);
        OscSenderReceiver::instance()->send(fConnectedDeviceIndex,
                                            CMD_SUBSCRIBE,
                                            fName,
                                            fSupportedCharacteristicIndex);
    }

    void unsubscribe() override {
        fPeripheral->unsubscribe(SERVICE_HEART_RATE,
                                 CHARACTERISTIC_HEART_RATE_MEASUREMENT_N);
    }

    void read() override {}

    void write() override {}

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE_HEART_RATE,
                                                       CHARACTERISTIC_HEART_RATE_MEASUREMENT_N,
                                                       [](
                                                               Peripheral *peripheral,
                                                               int connected_device_index,
                                                               int supported_characteristic_index) -> std::unique_ptr<CharacteristicAbstract> {
                                                           return std::make_unique<CharacteristicHeartRateMeasurment>(
                                                                   peripheral,
                                                                   connected_device_index,
                                                                   supported_characteristic_index);
                                                       });
    }

    const char *name() override { return fName; }

private:
    constexpr static const char *fName = "heartrate";

    void notify(ByteArray bytes) {
        // TODO read specs to parse payload properly
        const float mHeartRate = bytes[1];
        OscSenderReceiver::instance()->send(fConnectedDeviceIndex, fName, mHeartRate);
    }
};
