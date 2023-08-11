#pragma once

#include "simpleble/SimpleBLE.h"

// TODO add distinction between capabilities i.e notify(aka subscribe/unsubscribe), read, and write
// TODO rename to `Characteristic` and add namespace
// TODO move to extra folder

class CharacteristicAbstract {
public:
    CharacteristicAbstract(shared_ptr<SimpleBLE::Peripheral> peripheral,
                           int connected_device_index,
                           int supported_characteristic_index) :
            fPeripheral(std::move(peripheral)),
            fConnectedDeviceIndex(connected_device_index),
            fSupportedCharacteristicIndex(supported_characteristic_index) {}

    virtual ~CharacteristicAbstract() = default;

    virtual void
    init() = 0; /* is called once in the begining and can implement default behaviors i.e subscribing to notifications */

    virtual void
    cleanup() = 0; /* is called once in the end and can implement default behaviors i.e unsubscribing from notifications */

    virtual void subscribe() = 0; /* ... to notification */

    virtual void unsubscribe() = 0;  /* ... from notification */

    virtual void read() = 0;

    virtual void write(SimpleBLE::ByteArray bytes) = 0;

    virtual const char *name() = 0;

//    virtual bool is_supporting_command() = 0;
//
//    virtual bool command(std::string &typetag, std::vector<std::any> &message) = 0;

protected:
    shared_ptr<SimpleBLE::Peripheral> fPeripheral;
    const int                         fConnectedDeviceIndex;
    const int                         fSupportedCharacteristicIndex;

    void send(const int feature, const float value) const {
        Transceiver::instance()->send_feature_with_value(fConnectedDeviceIndex,
                                                         fSupportedCharacteristicIndex,
                                                         feature,
                                                         value);
    }
};
