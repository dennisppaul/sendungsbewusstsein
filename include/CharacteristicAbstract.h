#pragma once

#include "simpleble/SimpleBLE.h"

// TODO add distinction between capabilities i.e notify(aka subscribe/unsubscribe), read, and write
// TODO rename to `Characteristic` and add namespace
// TODO move to extra folder

class CharacteristicAbstract {
public:
    constexpr static const char *CMD_SUBSCRIBE = "subscribe";

    CharacteristicAbstract(SimpleBLE::Peripheral *peripheral,
                           int connected_device_index,
                           int supported_characteristic_index) :
            fPeripheral(peripheral),
            fConnectedDeviceIndex(connected_device_index),
            fSupportedCharacteristicIndex(supported_characteristic_index) {}

    virtual ~CharacteristicAbstract() = default;

    virtual void init() = 0; /* is called once and can implement default behaviors i.e subscribing to notifications */

    virtual void subscribe() = 0; /* ... to notification */

    virtual void unsubscribe() = 0;  /* ... from notification */

    virtual void read() = 0;

    virtual void write(SimpleBLE::ByteArray bytes) = 0;

    virtual const char *name() = 0;

protected:
    SimpleBLE::Peripheral *fPeripheral = nullptr;
    const int             fConnectedDeviceIndex;
    const int             fSupportedCharacteristicIndex;
};
