#pragma once

#include <utility>
#include "simpleble/SimpleBLE.h"
#include "utils.hpp"
#include "OscSenderReceiver.h"
#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"

using namespace SimpleBLE;
using namespace std;

//class Feature {
//public:
//    Feature(const char *feature_service,
//            const char *feature_characterisitic,
//            const char *feature_description) :
//            service(feature_service),
//            characteristic(feature_characterisitic),
//            description(feature_description) {};
//    const char *service;
//    const char *characteristic;
//    const char *description;
//};

class Device {
public:
    static constexpr const char *CMD_CONNECT    = "connect";
    static constexpr const char *CMD_DISCONNECT = "disconnect";

    Device(int osc_index, Peripheral *peripheral) : fID(osc_index),
                                                    fPeripheral(peripheral),
                                                    fName(peripheral->identifier()) {
        connect();
        update_services_and_characteristics();
        collect_supported_characteristics();
    }

    void collect_supported_characteristics() {
        for (auto &service: fPeripheral->services()) {
            for (auto &characteristic: service.characteristics()) {
                unique_ptr<CharacteristicAbstract> mCharacteristic = CharacteristicFactory::create(service.uuid(),
                                                                                                   characteristic.uuid(),
                                                                                                   fPeripheral,
                                                                                                   fID);
                if (mCharacteristic) {
                    console << "FOUND:: ";
                    console << "Service: " << service.uuid();
                    console << " Characteristic: " << characteristic.uuid();
                    console << " as '" << mCharacteristic->name() << "'";
                    console << endl;
                    mCharacteristic->subscribe();
                    supported_characeristics.push_back(std::move(mCharacteristic));
                }
            }
        }
    }

    void connect() {
        OscSenderReceiver::instance()->send(fID, CMD_CONNECT, fName.c_str());
        fPeripheral->connect();
    }

    void disconnect() {
        OscSenderReceiver::instance()->send(fID, CMD_DISCONNECT, fName.c_str());

    }

    std::string name() {
        return fName;
    }

    [[nodiscard]] int ID() const {
        return fID;
    }

private:
    std::string                                fName;
    const int                                  fID;
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