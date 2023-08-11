#pragma once

#include "Sendungsbewusstsein.h"
#include "Console.h"
#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"
#include "BitUtils.h"

using namespace SimpleBLE;

/* see "3.63 Cycling Power Control Point" p68ff from "GATT Specification Supplement (v10)" */

class CharacteristicCyclingPowerControlPoint : public CharacteristicAbstract {
public:
    CharacteristicCyclingPowerControlPoint(shared_ptr<SimpleBLE::Peripheral> peripheral,
                                           int connected_device_index,
                                           int supported_characteristic_index)
            : CharacteristicAbstract(peripheral,
                                     connected_device_index,
                                     supported_characteristic_index) {}

    void init() override {
        subscribe();
    }

    void cleanup() override {
        unsubscribe();
    }

    void subscribe() override {
        console << "CharacteristicCyclingPowerControlPoint::subscribe ( > indicate )" << endl;
        auto mCallback = bind( // NOLINT(*-avoid-bind)
                &CharacteristicCyclingPowerControlPoint::indicate,
                this,
                std::placeholders::_1);
        fPeripheral->indicate(SERVICE, CHARACTERISTIC, mCallback);
    }

    void unsubscribe() override {}

    void read() override {}

    void write(SimpleBLE::ByteArray bytes) override {
        // TODO: implement
        // write_request + indicate

        // NOTE: Alternatively, `write_command` can be used to write to a characteristic too.
        // `write_request` is for unacknowledged writes.
        // `write_command` is for acknowledged writes.
//        ByteArray bytes = {0x05}; // 0x05 = request crank length
//        fPeripheral->write_request(SERVICE, CHARACTERISTIC, bytes);
    }

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE, CHARACTERISTIC,
                                                       [](
                                                               const shared_ptr<SimpleBLE::Peripheral> &peripheral,
                                                               int connected_device_index,
                                                               int supported_characteristic_index) -> std::unique_ptr<CharacteristicAbstract> {
                                                           return std::make_unique<CharacteristicCyclingPowerControlPoint>(
                                                                   peripheral,
                                                                   connected_device_index,
                                                                   supported_characteristic_index);
                                                       });
    }

    const char *name() override { return fName; }

private:
    constexpr static const char *fName          = "cycling_power_control_point";
    constexpr static const char *SERVICE        = SERVICE_CYCLING_POWER;
    constexpr static const char *CHARACTERISTIC = CHARACTERISTIC_CYCLING_POWER_CONTROL_POINT_WRI;

    void indicate(ByteArray bytes) {
        // TODO e.g crank length response should be `ByteArray bytes = {0x20, }`
        console << "CharacteristicCyclingPowerControlPoint::indicate" << endl;
        Utils::print_byte_array_as_bits(bytes);
    }
};
