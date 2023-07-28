#pragma once

class CharacteristicAbstract {
public:
    virtual ~CharacteristicAbstract() = default;
    virtual void subscribe() = 0;
    virtual void unsubscribe() = 0;
    virtual void read() = 0;
    virtual void write() = 0;
};
