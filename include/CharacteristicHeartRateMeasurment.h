#pragma once

#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"

class CharacteristicHeartRateMeasurment : public CharacteristicAbstract {
public:

    CharacteristicHeartRateMeasurment(Peripheral *peripheral, int16_t ID)
            : CharacteristicAbstract(peripheral, ID) {}

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
                                                       [](Peripheral *peripheral,
                                                          int ID) -> std::unique_ptr<CharacteristicAbstract> {
                                                           return std::make_unique<CharacteristicHeartRateMeasurment>(
                                                                   peripheral, ID);
                                                       });
    }

    const char *name() override { return fName; }

private:
    constexpr static const char *fName = "heartrate";

    void notify(ByteArray bytes) {
        // TODO read specs to parse payload properly
        const float mHeartRate = bytes[1];
        OscSenderReceiver::instance()->send(fID, fName, mHeartRate);
    }
};
