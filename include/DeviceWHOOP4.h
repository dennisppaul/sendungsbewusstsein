#pragma once

#include <iostream>
#include <iomanip>

#include "simpleble/SimpleBLE.h"
#include "utils.hpp"
#include "OscSenderReceiver.h"
#include "Device.h"

using namespace SimpleBLE;
using namespace std;

class DeviceWHOOP4 : public Device {
#define WHOOP4_DEBUG_SERVICES
#define WHOOP4_DEBUG_HR

public:
    DeviceWHOOP4(int ID, Peripheral &peripheral) : fID(ID), fPeripheral(peripheral) {
        connect_impl();
        subscribe_heartrate();
    }

    ~DeviceWHOOP4() override = default;

    int ID() override {
        return fID;
    }

    const char *name() override {
        return NAME;
    }

    void connect() override { connect_impl(); }

    void disconnect() override {
        fPeripheral.disconnect();
        OscSenderReceiver::instance().send(NAME, fID, CMD_DISCONNECT);
    }

    static constexpr const char *NAME = "WHOOP 4A0934182";

private:
    static constexpr const char *SERVICE_WHOOP_CUSTOM                    = "61080001-8d6d-82b8-614a-1c8cb0f8dcc6";
    static constexpr const char *CHARACTERISTIC_01_WRWC                  = "61080002-8d6d-82b8-614a-1c8cb0f8dcc6";
    static constexpr const char *CHARACTERISTIC_02_N                     = "61080003-8d6d-82b8-614a-1c8cb0f8dcc6";
    static constexpr const char *CHARACTERISTIC_03_N                     = "61080004-8d6d-82b8-614a-1c8cb0f8dcc6";
    static constexpr const char *CHARACTERISTIC_04_N                     = "61080005-8d6d-82b8-614a-1c8cb0f8dcc6";
    static constexpr const char *CHARACTERISTIC_05_N                     = "61080007-8d6d-82b8-614a-1c8cb0f8dcc6";
    static constexpr const char *SERVICE_HEART_RATE                      = "0000180d-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_HEART_RATE_MEASUREMENT_N = "00002a37-0000-1000-8000-00805f9b34fb";
    static constexpr const char *SERVICE_DEVICE_INFORMATION_UUID         = "0000180a-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_MANUFACTURER_NAME_R      = "00002a29-0000-1000-8000-00805f9b34fb";
    static constexpr const char *SERVICE_BATTERY                         = "0000180f-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_BATTERY_LEVEL_RN         = "00002a19-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_DESCRIPTOR               = "00002902-0000-1000-8000-00805f9b34fb";

    const int                                  fID;
    Peripheral                                 &fPeripheral;
    vector<pair<BluetoothUUID, BluetoothUUID>> uuids;
    const Feature                              feature_heartrate{SERVICE_HEART_RATE,
                                                                 CHARACTERISTIC_HEART_RATE_MEASUREMENT_N,
                                                                 "heartrate"};

    void subscribe_heartrate() {
        // TODO maybe check if features are available
        /* heartreate */
        auto mHeartRateCallback = bind(&DeviceWHOOP4::notify_heartrate, // NOLINT(*-avoid-bind)
                                       this,
                                       std::placeholders::_1);
        fPeripheral.notify(feature_heartrate.service,
                           feature_heartrate.characteristic,
                           mHeartRateCallback);
    }

    void unsubscribe_heartrate() {
        fPeripheral.unsubscribe(feature_heartrate.service,
                                feature_heartrate.characteristic);
    }

    void notify_heartrate(ByteArray bytes) const {
#ifdef  WHOOP4_DEBUG_HR
        const float mHeartRate = bytes[1];
        cout << "HRM: ";
        cout << fixed << setprecision(0) << mHeartRate << " / ";
        Utils::print_byte_array(bytes);
#endif
        OscSenderReceiver::instance().send(NAME, fID, feature_heartrate.description, mHeartRate);
    }

    void update_services() {
        for (auto service: fPeripheral.services()) {
            for (auto characteristic: service.characteristics()) {
                uuids.emplace_back(service.uuid(), characteristic.uuid());
            }
        }
#ifdef  WHOOP4_DEBUG_SERVICES
        cout << "The following services and characteristics were found:" << endl;
        for (size_t j = 0; j < uuids.size(); j++) {
            cout << "[" << j << "] " << uuids[j].first << " " << uuids[j].second << endl;
        }
#endif

//        for (auto &service: fPeripheral.services()) {
//            cout << "Service: " << service.uuid() << endl;
//
//            for (auto &characteristic: service.characteristics()) {
//                cout << "  Characteristic: " << characteristic.uuid() << endl;
//
//                cout << "    Capabilities: ";
//                for (auto &capability: characteristic.capabilities()) {
//                    cout << capability << " ";
//                }
//                cout << endl;
//
//                for (auto &descriptor: characteristic.descriptors()) {
//                    cout << "    Descriptor: " << descriptor.uuid() << endl;
//                }
//            }
//        }
    }

    void connect_impl() {
        fPeripheral.connect();
        update_services();
        OscSenderReceiver::instance().send(NAME, fID, CMD_CONNECT);
    }
};

/*
Connecting to WHOOP 4A0934182 [D4E2AFC8-A8C0-46FC-CB3F-880251A0D10B]
Successfully connected.
MTU: 244
Service: 61080001-8d6d-82b8-614a-1c8cb0f8dcc6          "This is a custom service provided by the WHOOP heart rate monitor device."
  Characteristic: 61080002-8d6d-82b8-614a-1c8cb0f8dcc6 "This characteristic supports write operations using both write requests and write commands. It allows the central device to send data or commands to the heart rate monitor."
    Capabilities: write_request write_command
  Characteristic: 61080003-8d6d-82b8-614a-1c8cb0f8dcc6 "This characteristic supports the notify operation. The heart rate monitor can send notifications to the central device when certain events or conditions occur."
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 61080004-8d6d-82b8-614a-1c8cb0f8dcc6 "This characteristic also supports the notify operation. It allows the heart rate monitor to send notifications to the central device."
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 61080005-8d6d-82b8-614a-1c8cb0f8dcc6 "This characteristic supports the notify operation, enabling the heart rate monitor to send notifications to the central device."
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 61080007-8d6d-82b8-614a-1c8cb0f8dcc6 "This characteristic supports the notify operation, allowing the heart rate monitor to send notifications to the central device."
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
Service: 0000180d-0000-1000-8000-00805f9b34fb          "This is the standard Heart Rate service."
  Characteristic: 00002a37-0000-1000-8000-00805f9b34fb "This characteristic within the Heart Rate service supports the notify operation. It enables the heart rate monitor to send heart rate notifications to the central device."
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
Service: 0000180a-0000-1000-8000-00805f9b34fb          "This is the standard Device Information service."
  Characteristic: 00002a29-0000-1000-8000-00805f9b34fb "This characteristic within the Device Information service supports the read operation. The central device can request and read the value of this characteristic, which provides device information."
    Capabilities: read
Service: 0000180f-0000-1000-8000-00805f9b34fb          "This is the standard Battery service."
  Characteristic: 00002a19-0000-1000-8000-00805f9b34fb "This characteristic within the Battery service supports both read and notify operations. The central device can read the battery level information, and the heart rate monitor can send notifications when the battery level changes.
These attributes define the services and characteristics offered by the WHOOP heart rate monitor device, along with their corresponding capabilities such as read, write, and notify."
    Capabilities: read notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
 */

// *interpretations* of services and characteristics based on ChatGPT
