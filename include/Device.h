#pragma once

#include <utility>
#include "simpleble/SimpleBLE.h"
#include "utils.hpp"
#include "Transceiver.h"
#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"

using namespace SimpleBLE;
using namespace std;

class Device {
public:
    static constexpr const char *CMD_CONNECT                   = "connect";
    static constexpr const char *CMD_DISCONNECT                = "disconnect";
    static constexpr const char *CMD_SUPPORTED_CHARACTERISTICS = "supported_characteristics";

    Device(Peripheral *peripheral, int connected_device_index) :
            fPeripheral(peripheral),
            fConnectedDeviceIndex(connected_device_index),
            fName(peripheral->identifier()) {
        // TODO maybe setup `set_callback_on_connected` and `set_callback_on_disconnected` here
        peripheral->set_callback_on_connected([]() { console << "device connected" << std::endl; });
        peripheral->set_callback_on_disconnected([]() { console << "device disconnected" << std::endl; });

        connect();
        update_services_and_characteristics();
        collect_supported_characteristics();
    }

    ~Device() {
        disconnect();
    }

    void collect_supported_characteristics() {
        int       mSupportedCharacteristicIndex = -1;
        for (auto &service: fPeripheral->services()) {
            for (auto &characteristic: service.characteristics()) {
                mSupportedCharacteristicIndex++;
                auto mCharacteristic = CharacteristicFactory::create(service.uuid(),
                                                                     characteristic.uuid(),
                                                                     fPeripheral,
                                                                     fConnectedDeviceIndex,
                                                                     mSupportedCharacteristicIndex);
                if (mCharacteristic) {
                    console << "found supported characteristic: '"
                            << mCharacteristic->name()
                            << "' ( "
                            << "service: " << service.uuid()
                            << ", characteristic: " << characteristic.uuid()
                            << " )"
                            << endl;
                    mCharacteristic->init();
                    supported_characeristics.push_back(std::move(mCharacteristic));
                } else {
                    mSupportedCharacteristicIndex--;
                }
            }
        }
        Transceiver::instance()->send_characteristic_command(fConnectedDeviceIndex,
                                                             CMD_SUPPORTED_CHARACTERISTICS,
                                                                   mSupportedCharacteristicIndex + 1);
    }

    void connect() {
        console
                << "trying to connect to device '"
                << fName
                << "'"
                << endl;

        if (fPeripheral->is_connectable()) {
            Transceiver::instance()->send_device_info(fConnectedDeviceIndex, CMD_CONNECT, fName.c_str());
            fPeripheral->connect();
        } else {
            console
                    << "device '"
                    << fName
                    << "' is not connectable"
                    << endl;
        }
    }

    void disconnect() {
        console
                << "trying to disconnect from device '"
                << fName
                << "'"
                << endl;

        if (fPeripheral->is_connected()) {
            Transceiver::instance()->send_device_info(fConnectedDeviceIndex, CMD_DISCONNECT, fName.c_str());
            fPeripheral->disconnect();
        }
    }

    std::string name() {
        return fName;
    }

    [[nodiscard]] int ID() const {
        return fConnectedDeviceIndex;
    }

    bool has_supported_characteristics() {
        return !supported_characeristics.empty();
    }

    bool is_connected() {
        return fPeripheral->is_connected();
    }

private:
    std::string                                fName;
    const int                                  fConnectedDeviceIndex;
    Peripheral                                 *fPeripheral;
    vector<pair<BluetoothUUID, BluetoothUUID>> uuids;
    vector<unique_ptr<CharacteristicAbstract>> supported_characeristics;

    void update_services_and_characteristics() {
        for (auto service: fPeripheral->services()) {
            for (auto characteristic: service.characteristics()) {
                uuids.emplace_back(service.uuid(), characteristic.uuid());
            }
        }
        for (auto &service: fPeripheral->services()) {
            console << "Service: " << service.uuid() << endl;

            for (auto &characteristic: service.characteristics()) {
                console << "  Characteristic: " << characteristic.uuid() << endl;

                console << "    Capabilities: ";
                for (auto &capability: characteristic.capabilities()) {
                    console << capability << " ";
                }
                console << endl;

                for (auto &descriptor: characteristic.descriptors()) {
                    console << "    Descriptor: " << descriptor.uuid() << endl;
                }
            }
        }
    }
};