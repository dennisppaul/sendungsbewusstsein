#pragma once

class Device {
public:
    virtual ~Device() = default;
    virtual int ID() = 0;
};