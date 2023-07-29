#pragma once

#include "simpleble/SimpleBLE.h"

using namespace SimpleBLE;
using namespace std;

class CharacteristicAbstract {
public:
    CharacteristicAbstract(Peripheral *peripheral, int ID) : fPeripheral(peripheral), fID(ID) {}

    virtual ~CharacteristicAbstract() = default;

    virtual void subscribe() = 0;

    virtual void unsubscribe() = 0;

    virtual void read() = 0;

    virtual void write() = 0;

    virtual const char *name() = 0;

protected:
    Peripheral *fPeripheral = nullptr;
    const int  fID;
};
