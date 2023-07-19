#include <utility>

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

    Device(std::string name, int osc_index) : fName(std::move(name)), fID(osc_index) {}

    Device(const Device &) = delete;

    virtual ~Device() = default;

    virtual void connect() = 0;

    virtual void disconnect() = 0;

    std::string name() {
        return fName;
    }

    [[nodiscard]] int ID() const {
        return fID;
    }

private:
    const std::string fName;
    const int         fID;
};