/*
Service: 00001816-0000-1000-8000-00805f9b34fb
  Characteristic: 00002a5b-0000-1000-8000-00805f9b34fb
    Capabilities: notify,
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002a5c-0000-1000-8000-00805f9b34fb
    Capabilities: read,
  Characteristic: 00002a5d-0000-1000-8000-00805f9b34fb
    Capabilities: read,
  Characteristic: 00002a55-0000-1000-8000-00805f9b34fb
    Capabilities: write_request, indicate,
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
*/

#pragma once

#include <utility>

#include "Sendungsbewusstsein.h"
#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"
#include "BitUtils.h"

using namespace SimpleBLE;
using namespace std;

class CharacteristicCyclingSpeedCadence : public CharacteristicAbstract {
public:
    CharacteristicCyclingSpeedCadence(shared_ptr<SimpleBLE::Peripheral> peripheral,
                                      int connected_device_index,
                                      int supported_characteristic_index)
            : CharacteristicAbstract(std::move(peripheral),
                                     connected_device_index,
                                     supported_characteristic_index) {
        fFeatures.emplace_back(FEATURE_WHEEL_STR);
        fFeatures.emplace_back(FEATURE_CRANK_STR);
    }

    void init() override {

        subscribe();
    }

    void cleanup() override {
        unsubscribe();
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
                &CharacteristicCyclingSpeedCadence::notify,
                this,
                std::placeholders::_1);
        fPeripheral->notify(SERVICE, CHARACTERISTIC, mCallback);
        Transceiver::instance()->send_characteristic_information_with_value(fConnectedDeviceIndex,
                                                                            fSupportedCharacteristicIndex,
                                                                            SUBSCRIBED,
                                                                            NUM_FEATURES);
    }

    void unsubscribe() override {
        console
                << "unsubscribing from '"
                << name()
                << "' "
                << "as service "
                << SERVICE
                << " with characteristic "
                << CHARACTERISTIC
                << std::endl;

        try {
            fPeripheral->unsubscribe(SERVICE, CHARACTERISTIC);
        } catch (const exception &e) {
            // TODO not sure if this is still necessary … there is a potential problem in 'PeripheralBaseMacOS.mm' with premature failure to unsubscribe
            console << "could not unsubscripe" << endl;
        }

        Transceiver::instance()->send_characteristic_information(fConnectedDeviceIndex,
                                                                 fSupportedCharacteristicIndex,
                                                                 UNSUBSCRIBED);
    }

    void read() override {}

    void write(SimpleBLE::ByteArray bytes) override {}

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE, CHARACTERISTIC,
                                                       [](
                                                               const shared_ptr<SimpleBLE::Peripheral> &peripheral,
                                                               int connected_device_index,
                                                               int supported_characteristic_index) -> std::unique_ptr<CharacteristicAbstract> {
                                                           return std::make_unique<CharacteristicCyclingSpeedCadence>(
                                                                   peripheral,
                                                                   connected_device_index,
                                                                   supported_characteristic_index);
                                                       });
    }

    const char *name() override { return fName; }

private:
    constexpr static const char *fName          = "cycling_speed_cadence";
    constexpr static const char *SERVICE        = SERVICE_CYCLING_SPEED_AND_CADENCE;
    constexpr static const char *CHARACTERISTIC = CHARACTERISTIC_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_N;

    // TODO this needs to be sorted out properly
    static const int            FEATURE_WHEEL      = 0;
    constexpr static const char *FEATURE_WHEEL_STR = "wheel";
    static const int            FEATURE_CRANK      = 1;
    constexpr static const char *FEATURE_CRANK_STR = "crank";
    static const int            NUM_FEATURES       = 2;

    enum {
        FLAG_WHEEL_REVOLUTION_DATA_PRESENT = 0x0001,
        FLAG_CRANK_REVOLUTION_DATA_PRESENT = 0x0002,
    };

    void notify(ByteArray bytes) {
        // 3.61 CSC Measurement
        uint8_t flags = bytes[0];
        int     i     = 1;

        if (flags & FLAG_WHEEL_REVOLUTION_DATA_PRESENT) {
            // 3.61.2 Wheel Revolution Data field
            uint32_t mCumulativeWheelRevolutions = bytes_to_uint32(bytes[i + 3], bytes[i + 2], bytes[i + 1], bytes[i]);
            uint16_t mLastWheelEventTime         = bytes_to_uint16(bytes[i + 5],
                                                                   bytes[i + 4]); // Unit is 1/1024th of a second
            send(FEATURE_WHEEL, static_cast<float>(mCumulativeWheelRevolutions));
        }
        if (flags & FLAG_CRANK_REVOLUTION_DATA_PRESENT) {
            // 3.61.3 Crank Revolution Data field
            uint16_t mCumulativeCrankRevolutions = bytes_to_uint16(bytes[i + 1], bytes[i]);
            uint16_t mLastCrankEventTime         = bytes_to_uint16(bytes[i + 3],
                                                                   bytes[i + 2]); // Unit is 1/1024th of a second
            send(FEATURE_CRANK, static_cast<float>(mCumulativeCrankRevolutions));
        }
#ifdef DEBUG_CYCLING_SPEED_AND_CADENCE_MEASUREMENT
        console << "CyclingSpeedCadence:" << endl;
        Utils::print_byte_array_as_bits(bytes);
#endif // DEBUG_CYCLING_SPEED_AND_CADENCE_MEASUREMENT
    }
};
