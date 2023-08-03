#pragma once

#include "SendungsbewusstseinDefines.h"
#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"
#include "BitUtils.h"

using namespace SimpleBLE;

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
                << "subscribing to '"
                << name()
                << "' "
                << "as service "
                << SERVICE
                << " with characteristic "
                << CHARACTERISTIC
                << std::endl;

        auto mCallback = bind( // NOLINT(*-avoid-bind)
                &CharacteristicHeartRateMeasurment::notify,
                this,
                std::placeholders::_1);
        fPeripheral->notify(SERVICE, CHARACTERISTIC, mCallback);
        OscSenderReceiver::instance()->send_characteristic_command(fConnectedDeviceIndex,
                                                                   CMD_SUBSCRIBE,
                                                                   fName,
                                                                   fSupportedCharacteristicIndex);
    }

    void unsubscribe() override { fPeripheral->unsubscribe(SERVICE, CHARACTERISTIC); }

    void read() override {}

    void write() override {}

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE, CHARACTERISTIC,
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
    constexpr static const char *fName                 = "heartrate_rate_measurement";
    constexpr static const char *SERVICE               = SERVICE_HEART_RATE;
    constexpr static const char *CHARACTERISTIC        = CHARACTERISTIC_HEART_RATE_MEASUREMENT_N;
    constexpr static const char *FEATURE_STR_HEARTRATE = "heartrate";

    enum {
        FLAG_HEART_RATE_VALUE_FORMAT  = 0x0001,
        FLAG_SENSOR_CONTACT_DETECTED  = 0x0002,
        FLAG_SENSOR_CONTACT_SUPPORTED = 0x0004,
        FLAG_ENERGY_EXPANDED_PRESENT  = 0x0008,
        FLAG_RR_INTERVAL_PRESENT      = 0x0010,
    };

    void notify(ByteArray bytes) {
        // TODO read specs to parse payload properly
        // see '3.114 Heart Rate Measurement' from 'GATT_Specification_Supplement_v10'
        float   mHeartRate;
        uint8_t flags = bytes[0];

        int i = 1;
        if (flags & FLAG_HEART_RATE_VALUE_FORMAT) {
            mHeartRate = bytes_to_uint16(bytes[i + 1], bytes[i]);
            i += 2;
        } else {
            mHeartRate = bytes[i];
            i += 1;
        }
#ifdef DEBUG_HEART_RATE_MEASUREMENT_FEATURES
        console << "HeartRateMeasurement:" << endl;
        Utils::print_byte_array_as_bits(bytes);
#endif // DEBUG_HEART_RATE_MEASUREMENT_FEATURES
        OscSenderReceiver::instance()->send_characteristic_value_with_feature(fConnectedDeviceIndex,
                                                                              fName,
                                                                              FEATURE_STR_HEARTRATE,
                                                                              mHeartRate);
    }
};
