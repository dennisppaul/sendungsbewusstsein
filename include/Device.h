#pragma once

class Feature {
public:
    Feature(const char *feature_service,
            const char *feature_characterisitic,
            const char *feature_description) :
            service(feature_service),
            characteristic(feature_characterisitic),
            description(feature_description) {};
    const char *service;
    const char *characteristic;
    const char *description;
};

class Device {
public:
    static constexpr const char *CMD_CONNECT    = "connect";
    static constexpr const char *CMD_DISCONNECT = "disconnect";

    virtual ~Device() = default;

    virtual int ID() = 0;

    virtual const char *name() = 0;

    virtual void connect() = 0;

    virtual void disconnect() = 0;
};