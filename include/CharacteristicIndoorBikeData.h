#pragma once

#include "SendungsbewusstseinDefines.h"
#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"
#include "BitUtils.h"
#include "utils.hpp"

using namespace SimpleBLE;

class CharacteristicIndoorBikeData : public CharacteristicAbstract {
public:
    CharacteristicIndoorBikeData(Peripheral *peripheral,
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
                << "( service: "
                << SERVICE
                << " with characteristic: "
                << CHARACTERISTIC
                << " )"
                << std::endl;

        auto mCallback = bind( // NOLINT(*-avoid-bind)
                &CharacteristicIndoorBikeData::notify,
                this,
                std::placeholders::_1);
        fPeripheral->notify(SERVICE, CHARACTERISTIC, mCallback);
        // TODO `NUM_FEATURES` maybe needs to be querried from characteristic?
        Transceiver::instance()->send_characteristic_information_with_value(fConnectedDeviceIndex,
                                                                            fSupportedCharacteristicIndex,
                                                                            SUBSCRIBED,
                                                                            NUM_FEATURES);
    }

    void unsubscribe() override {
        fPeripheral->unsubscribe(SERVICE, CHARACTERISTIC);
        Transceiver::instance()->send_characteristic_information(fConnectedDeviceIndex,
                                                                 fSupportedCharacteristicIndex,
                                                                 UNSUBSCRIBED);
    }

    void read() override {}

    void write(SimpleBLE::ByteArray bytes) override {}

    const char *name() override { return fName; }

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE, CHARACTERISTIC,
                                                       [](
                                                               Peripheral *peripheral,
                                                               int connected_device_index,
                                                               int supported_characteristic_index) -> std::unique_ptr<CharacteristicAbstract> {
                                                           return std::make_unique<CharacteristicIndoorBikeData>(
                                                                   peripheral,
                                                                   connected_device_index,
                                                                   supported_characteristic_index);
                                                       });
    }

private:
    constexpr static const char *fName          = "indoor_bike_data";
    constexpr static const char *SERVICE        = SERVICE_FITNESS_MACHINE;
    constexpr static const char *CHARACTERISTIC = CHARACTERISTIC_INDOOR_BIKE_DATA_N;

    static const int            FEATURE_SPEED_ENUM   = 0;
    constexpr static const char *FEATURE_SPEED_STR   = "speed";
    static const int            FEATURE_POWER_ENUM   = 1;
    constexpr static const char *FEATURE_POWER_STR   = "power";
    static const int            FEATURE_CADENCE_ENUM = 2;
    constexpr static const char *FEATURE_CADENCE_STR = "cadence";
    static const int            NUM_FEATURES         = 3;

    /*
    | Bit | Definition            |
    |-----|-----------------------|
    | 0   | More Data             |
    | 1   | Average Speed         |
    | 2   | Instantaneous Cadence |
    | 3   | Average Cadence       |
    | 4   | Total Distance        |
    | 5   | Resistance Level      |
    | 6   | Instantaneous Power   |
    | 7   | Average Power         |
    | 8   | Expended Energy       |
    | 9   | Heart Rate            |
    | 10  | Metabolic Equivalent  |
    | 11  | Elapsed Time          |
    | 12  | Remaining Time        |
    | 13  | UNUSED                |
    | 14  | UNUSED                |
    | 15  | UNUSED                |
    */
    enum {
        FLAG_MORE_DATA             = 0x0001,
        FLAG_INSTANTANEOUS_SPEED   = 0x0002,
        FLAG_INSTANTANEOUS_CADENCE = 0x0004,
        FLAG_AVERAGE_CADENCE       = 0x0008,
        FLAG_TOTAL_DISTANCE        = 0x0010,
        FLAG_RESISTANCE_LEVEL      = 0x0020,
        FLAG_INSTANTANEOUS_POWER   = 0x0040,
        FLAG_AVERAGE_POWER         = 0x0080,
        FLAG_EXPENDED_ENERGY       = 0x0100,
        FLAG_HEART_RATE            = 0x0200,
        FLAG_METABOLIC_EQUIVALENT  = 0x0400,
        FLAG_ELAPSED_TIME          = 0x0800,
        FLAG_REMAINING_TIME        = 0x1000,
    };

    static void print_features(uint16_t flags) {
        if (flags & FLAG_MORE_DATA) {
            console << "FLAG_MORE_DATA" << endl;
        }
        if (!(flags & FLAG_INSTANTANEOUS_SPEED)) {
            console << "FLAG_INSTANTANEOUS_SPEED" << endl;
        }
        if (flags & FLAG_INSTANTANEOUS_CADENCE) {
            console << "FLAG_INSTANTANEOUS_CADENCE" << endl;
        }
        if (flags & FLAG_AVERAGE_CADENCE) {
            console << "FLAG_AVERAGE_CADENCE" << endl;
        }
        if (flags & FLAG_TOTAL_DISTANCE) {
            console << "FLAG_TOTAL_DISTANCE" << endl;
        }
        if (flags & FLAG_RESISTANCE_LEVEL) {
            console << "FLAG_RESISTANCE_LEVEL" << endl;
        }
        if (flags & FLAG_INSTANTANEOUS_POWER) {
            console << "FLAG_INSTANTANEOUS_POWER" << endl;
        }
        if (flags & FLAG_AVERAGE_POWER) {
            console << "FLAG_AVERAGE_POWER" << endl;
        }
        if (flags & FLAG_EXPENDED_ENERGY) {
            console << "FLAG_EXPENDED_ENERGY" << endl;
        }
        if (flags & FLAG_HEART_RATE) {
            console << "FLAG_HEART_RATE" << endl;
        }
        if (flags & FLAG_METABOLIC_EQUIVALENT) {
            console << "FLAG_METABOLIC_EQUIVALENT" << endl;
        }
        if (flags & FLAG_ELAPSED_TIME) {
            console << "FLAG_ELAPSED_TIME" << endl;
        }
        if (flags & FLAG_REMAINING_TIME) {
            console << "FLAG_REMAINING_TIME" << endl;
        }
    }

    void notify(ByteArray bytes) {

        uint16_t flags = bytes_to_uint16(bytes[1], bytes[0]);
#ifdef DEBUG_INDOOR_BIKE_DATA_FEATURES
        console << "IndoorBikeData:" << endl;
        Utils::print_byte_array_as_bits(bytes);
        print_features(flags);
#endif // DEBUG_INDOOR_BIKE_DATA_FEATURES

        /*
        | Parameter             | Format | Unit       |
        |-----------------------|--------|------------|
        | Instantaneous Speed   | uint16 | 0.01 Km/h  |
        | Average Speed         | uint16 | 0.01 Km/h  |
        | Instantaneous Cadence | uint16 | 0.5 /min   |
        | Average Cadence       | uint16 | 0.5 /min   |
        | Total Distance        | uint24 | metre      |
        | Resistance Level      | unit8  | 1 unitless |
        | Instantaneous Power   | sint16 | 1 watt     |
        | Average Power         | sint16 | 1 watt     |
        | Total Energy          | uint16 | kcal       |
        | Energy Per Hour       | uint16 | kcal       |
        | Energy Per Minute     | uint8  | kcal       |
        | Heart Rate            | uint8  | Beats/m    |
        | Metabolic Equivalent  | uint8  | 0.1 meta   |
        | Elapsed Time          | uint16 | second     |
        | Remaining Time        | uint16 | second     |
        */

        // example from 'Wahoo KICKR':
        // 00100010 00000000 00000000 00000000 00000000 00000000 00000000 00000000
        // |- FLAGS -------| |- AVG SPEED ---| |- CADENCE -----| |- POWER -------|

        int i = 2;
        if (flags & FLAG_MORE_DATA) {
        }
        if (!(flags & FLAG_INSTANTANEOUS_SPEED)) {
            //         | Instantaneous Speed   | uint16 | 0.01 Km/h  |
            float speed = (float) bytes_to_uint16(bytes[i + 1], bytes[i]) * 0.01f;
            send(FEATURE_SPEED_ENUM, speed);
            i += 2;
        }
        if (flags & FLAG_INSTANTANEOUS_CADENCE) {
            //         | Instantaneous Cadence | uint16 | 0.5 /min   |
            float cadence = (float) bytes_to_uint16(bytes[i + 1], bytes[i]) * 0.5f;
            send(FEATURE_CADENCE_ENUM, cadence);
            i += 2;
        }
        if (flags & FLAG_AVERAGE_CADENCE) {
            //         | Average Cadence       | uint16 | 0.5 /min   |
            i += 2;
        }
        if (flags & FLAG_TOTAL_DISTANCE) {
            //         | Total Distance        | uint24 | metre      |
            i += 3;
        }
        if (flags & FLAG_RESISTANCE_LEVEL) {
            //         | Resistance Level      | unit8  | 1 unitless |
            i += 1;
        }
        if (flags & FLAG_INSTANTANEOUS_POWER) {
            //         | Instantaneous Power   | sint16 | 1 watt     |
            float power = bytes_to_int16(bytes[i + 1], bytes[i]);
            send(FEATURE_POWER_ENUM, power);
            i += 2;
        }
        if (flags & FLAG_AVERAGE_POWER) {
            //         | Average Power         | sint16 | 1 watt     |
            i += 2;
        }
        if (flags & FLAG_EXPENDED_ENERGY) {
            //         | Total Energy          | uint16 | kcal       |
            //         | Energy Per Hour       | uint16 | kcal       |
            //         | Energy Per Minute     | uint8  | kcal       |
            i += 5;
        }
        if (flags & FLAG_HEART_RATE) {
            //         | Heart Rate            | uint8  | Beats/m    |
            i += 1;
        }
        if (flags & FLAG_METABOLIC_EQUIVALENT) {
            //         | Metabolic Equivalent  | uint8  | 0.1 meta   |
            i += 1;
        }
        if (flags & FLAG_ELAPSED_TIME) {
            //         | Elapsed Time          | uint16 | second     |
            i += 2;
        }
        if (flags & FLAG_REMAINING_TIME) {
            //         | Remaining Time        | uint16 | second     |
        }
    }
};
