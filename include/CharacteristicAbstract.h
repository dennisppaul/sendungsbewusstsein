#pragma once

#include "simpleble/SimpleBLE.h"

using namespace SimpleBLE;

// TODO add distinction between capabilities i.e notify(aka subscribe/unsubscribe), read, and write

class CharacteristicAbstract {
public:
    constexpr static const char *CMD_SUBSCRIBE = "subscribe";

    CharacteristicAbstract(Peripheral *peripheral,
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

    virtual void write() = 0;

    virtual const char *name() = 0;

protected:
    Peripheral *fPeripheral = nullptr;
    const int  fConnectedDeviceIndex;
    const int  fSupportedCharacteristicIndex;
};
