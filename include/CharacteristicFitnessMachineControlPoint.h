#pragma once

#include "SendungsbewusstseinDefines.h"
#include "Console.h"
#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"
#include "BitUtils.h"

using namespace SimpleBLE;

/* see "4.16 Fitness Machine Control Point" p50ff from "Fitness Machine Service (v1.0)" */

class CharacteristicFitnessMachineControlPoint : public CharacteristicAbstract {
public:
    CharacteristicFitnessMachineControlPoint(Peripheral *peripheral,
                                           int connected_device_index,
                                           int supported_characteristic_index)
            : CharacteristicAbstract(peripheral,
                                     connected_device_index,
                                     supported_characteristic_index) {}

    void init() override {
// cycling power
// +++ Service: 00001818-0000-1000-8000-00805f9b34fb
// +++   Characteristic: 00002a63-0000-1000-8000-00805f9b34fb
// +++     Capabilities: notify
// +++     Descriptor: 00002902-0000-1000-8000-00805f9b34fb
//
//        console
//                << "XXXXX trying something crazy here:"
//                << endl;
//        SimpleBLE::ByteArray rx_data = fPeripheral->read("00001818-0000-1000-8000-00805f9b34fb",
//                                                         "00002a63-0000-1000-8000-00805f9b34fb",
//                                                         "00002902-0000-1000-8000-00805f9b34fb");
//        console << "Characteristic content is: " << endl;
//        Utils::print_byte_array(rx_data);
    }

    void subscribe() override {}

    void unsubscribe() override {}

    void read() override {}

    void write() override {
        // TODO: implement
        // write_request + indicate

        // NOTE: Alternatively, `write_command` can be used to write to a characteristic too.
        // `write_request` is for unacknowledged writes.
        // `write_command` is for acknowledged writes.
        ByteArray bytes = {0};
        fPeripheral->write_request(SERVICE, CHARACTERISTIC, bytes);
    }

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE, CHARACTERISTIC,
                                                       [](
                                                               Peripheral *peripheral,
                                                               int connected_device_index,
                                                               int supported_characteristic_index) -> std::unique_ptr<CharacteristicAbstract> {
                                                           return std::make_unique<CharacteristicFitnessMachineControlPoint>(
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
};
