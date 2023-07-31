#pragma once

#include "simpleble/SimpleBLE.h"

using namespace SimpleBLE;
using namespace std;

// TODO add distinction between notify(aka subscribe/unsubscribe), read, and write

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

    virtual void init() = 0;

    virtual void subscribe() = 0;

    virtual void unsubscribe() = 0;

    virtual void read() = 0;

    virtual void write() = 0;

    virtual const char *name() = 0;

protected:
    Peripheral *fPeripheral = nullptr;
    const int  fConnectedDeviceIndex;
    const int  fSupportedCharacteristicIndex;
};
