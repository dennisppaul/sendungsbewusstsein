#pragma once

#include "Device.h"

class DeviceWahooKICKR : public Device {
public:
    static constexpr const char *NAME = "Wahoo KICKR 3724";

    DeviceWahooKICKR(int ID, Peripheral &peripheral) : fID(ID), fPeripheral(peripheral) {
        connect_impl();
        subscribe_cycling_power_measurement();
//        subscribe_indoor_bike_data();
    }

    ~DeviceWahooKICKR() override = default;

    int ID() override {
        return fID;
    }

    const char *name() override {
        return NAME;
    }

    void connect() override { connect_impl(); }

    void disconnect() override {
        fPeripheral.disconnect();
        OscSenderReceiver::instance()->send(NAME, fID, CMD_DISCONNECT);
    }

private:
    const int                                  fID;
    Peripheral                                 &fPeripheral;
    vector<pair<BluetoothUUID, BluetoothUUID>> uuids;

    void update_services() {
        for (auto service: fPeripheral.services()) {
            for (auto characteristic: service.characteristics()) {
                uuids.emplace_back(service.uuid(), characteristic.uuid());
            }
        }
        for (auto &service: fPeripheral.services()) {
            cout << "Service: " << service.uuid() << endl;

            for (auto &characteristic: service.characteristics()) {
                cout << "  Characteristic: " << characteristic.uuid() << endl;

                cout << "    Capabilities: ";
                for (auto &capability: characteristic.capabilities()) {
                    cout << capability << " ";
                }
                cout << endl;

                for (auto &descriptor: characteristic.descriptors()) {
                    cout << "    Descriptor: " << descriptor.uuid() << endl;
                }
            }
        }
    }

    void connect_impl() {
        fPeripheral.connect();
        update_services();
        OscSenderReceiver::instance()->send(NAME, fID, CMD_CONNECT);

        cout << "+++ reading CHARACTERISTIC_FITNESS_MACHINE_FEATURE_R: ";
        Utils::print_byte_array_as_bits(
                fPeripheral.read(SERVICE_FITNESS_MACHINE, CHARACTERISTIC_FITNESS_MACHINE_FEATURE_R));
        Utils::print_byte_array(fPeripheral.read(SERVICE_FITNESS_MACHINE, CHARACTERISTIC_FITNESS_MACHINE_FEATURE_R));
        cout << "+++ reading CHARACTERISTIC_CYCLING_POWER_FEATURE_R  : ";
        Utils::print_byte_array_as_bits(
                fPeripheral.read(SERVICE_CYCLING_POWER, CHARACTERISTIC_CYCLING_POWER_FEATURE_R));
        Utils::print_byte_array(fPeripheral.read(SERVICE_CYCLING_POWER, CHARACTERISTIC_CYCLING_POWER_FEATURE_R));
        cout << endl;
    }

    static constexpr const char *SERVICE_CYCLING_POWER                             = "00001818-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N        = "00002a63-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_CYCLING_POWER_FEATURE_R            = "00002a65-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_SENSOR_LOCATION_R                  = "00002a5d-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_WAHOO_TRAINER_WRI                  = "a026e005-0a7d-4ab3-97fa-f1500f9feb8b";
    static constexpr const char *CHARACTERISTIC_CYCLING_POWER_CONTROL_POINT_WRI    = "00002a66-0000-1000-8000-00805f9b34fb";
    static constexpr const char *SERVICE_FITNESS_MACHINE                           = "00001826-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_FITNESS_MACHINE_FEATURE_R          = "00002acc-0000-1000-8000-00805f9b34fb"; // "Fitness Machine Service", p19
    static constexpr const char *CHARACTERISTIC_TRAINING_STATUS_RN                 = "00002ad3-0000-1000-8000-00805f9b34fb"; // "Fitness Machine Service", p46
    static constexpr const char *CHARACTERISTIC_FITNESS_MACHINE_CONTROLL_POINT_WRI = "00002ad9-0000-1000-8000-00805f9b34fb"; // "Fitness Machine Service", p50
    static constexpr const char *CHARACTERISTIC_FITNESS_MACHINE_STATUS_N           = "00002ada-0000-1000-8000-00805f9b34fb"; // "Fitness Machine Service", p66
    static constexpr const char *CHARACTERISTIC_INDOOR_BIKE_DATA_N                 = "00002ad2-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_SUPPORTED_RESISTENCE_LEVEL_RANGE_R = "00002ad6-0000-1000-8000-00805f9b34fb";
    static constexpr const char *CHARACTERISTIC_SUPPORTED_POWER_RANGE_R            = "00002ad8-0000-1000-8000-00805f9b34fb";

    /* see "Fitness Machine Service", p50ff (https://www.bluetooth.com/specifications/specs/fitness-machine-service-1-0/) */
    /* see https://www.youtube.com/watch?v=AokDN6r4iz8 */
    const Feature feature_indoor_bike_data{SERVICE_FITNESS_MACHINE,
                                           CHARACTERISTIC_INDOOR_BIKE_DATA_N,
                                           "indoor_bike_data"};

    void subscribe_indoor_bike_data() {
        // TODO maybe check if features are available
        /* heartreate */
        auto mCallback = bind(&DeviceWahooKICKR::notify_indoor_bike_data, // NOLINT(*-avoid-bind)
                              this,
                              placeholders::_1);
        fPeripheral.notify(feature_indoor_bike_data.service,
                           feature_indoor_bike_data.characteristic,
                           mCallback);
    }

    void unsubscribe_indoor_bike_data() {
        fPeripheral.unsubscribe(feature_indoor_bike_data.service,
                                feature_indoor_bike_data.characteristic);
    }


    void notify_indoor_bike_data(ByteArray bytes) const {
#define  WAHOO_KICKR_DEBUG_BIKE_DATA
#ifdef  WAHOO_KICKR_DEBUG_BIKE_DATA
        cout << "NOTIFICATION: ";
//        cout << fixed << setprecision(0) << mHeartRate << " / ";
        Utils::print_byte_array(bytes);
        cout << "FLAG        : ";
        Utils::print_byte(bytes[0]);
        cout << " ";
        Utils::print_byte(bytes[1]);
        cout << endl;
#endif
        OscSenderReceiver::instance()->send(NAME, fID, feature_indoor_bike_data.description, 0.0f);
    }


    const Feature feature_cycling_power_measurement{SERVICE_CYCLING_POWER,
                                                    CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N,
                                                    "cycling_power_measurement"};

    void subscribe_cycling_power_measurement() {
        // TODO maybe check if features are available
        /* heartreate */
        auto mCallback = bind(&DeviceWahooKICKR::notify_cycling_power_measurement, // NOLINT(*-avoid-bind)
                              this,
                              placeholders::_1);
        fPeripheral.notify(feature_cycling_power_measurement.service,
                           feature_cycling_power_measurement.characteristic,
                           mCallback);
    }

    void unsubscribe_cycling_power_measurement() {
        fPeripheral.unsubscribe(feature_cycling_power_measurement.service,
                                feature_cycling_power_measurement.characteristic);
    }


    void notify_cycling_power_measurement(ByteArray bytes) const {
        // TODO this is just a quick dirty hack
        // TODO is the byte order reversed?
//        uint16_t flags                        = (bytes[0] << 8) | bytes[1];
//        auto instantaneous_power = static_cast<int16_t>((static_cast<uint16_t>(bytes[3]) << 8) | (bytes[2]));
        int16_t instantaneous_power = ((int16_t) bytes[3] * 256) + (uint8_t) bytes[2]; // TODO this is just a quick dirty hack try `int16_t instantaneous_power = (bytes[3] << 8) | bytes[2];`
//        uint16_t accumulated_torque           = (bytes[4] << 8) | bytes[5];
//        uint32_t cumulative_wheel_revolutions = (bytes[6] << 24) | (bytes[7] << 16) | (bytes[8] << 8) | bytes[9];
//        uint16_t last_wheel_event_time        = (bytes[10] << 8) | bytes[11];
//        uint16_t cumulative_crank_revolutions = (bytes[12] << 8) | bytes[13];
//        uint16_t last_crank_event_time        = (bytes[14] << 8) | bytes[15];
#define  WAHOO_KICKR_DEBUG_PWR
#ifdef  WAHOO_KICKR_DEBUG_PWR
        cout << dec;
        cout << "instantaneous_power: " << instantaneous_power;
//        cout << " \t ";
//        cout << "accumulated_torque: " << accumulated_torque;
//        cout << " \t ";
//        cout << "cumulative_wheel_revolutions: " << cumulative_wheel_revolutions;
//        cout << " \t ";
//        cout << "cumulative_crank_revolutions: " << cumulative_crank_revolutions;
        cout << endl;

//        const float mHeartRate = bytes[1];
//        cout << "PWR: ";
//        cout << fixed << setprecision(0) << mHeartRate << " / ";
//        Utils::print_byte_array(bytes);
#endif
        OscSenderReceiver::instance()->send(NAME, fID, feature_cycling_power_measurement.description,
                                           instantaneous_power);
    }
};

/*
Connecting to Wahoo KICKR 3724 [4D250578-36AF-BE46-B680-3BF913009F67]
Successfully connected.
MTU: 20
Service: 0000180a-0000-1000-8000-00805f9b34fb          "This is the standard Device Information service."
  Characteristic: 00002a29-0000-1000-8000-00805f9b34fb "This characteristic within the Device Information service supports the read operation. The central device can request and read the value of this characteristic, which provides device information."
    Capabilities: read
  Characteristic: 00002a25-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: 00002a27-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: 00002a26-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
Service: a026ee01-0a7d-4ab3-97fa-f1500f9feb8b
  Characteristic: a026e002-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: write_command notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: a026e004-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: a026e03b-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: write_command notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
Service: a026ee03-0a7d-4ab3-97fa-f1500f9feb8b          ""
  Characteristic: a026e00a-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: write_command notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
Service: 00001818-0000-1000-8000-00805f9b34fb          ""
  Characteristic: 00002a63-0000-1000-8000-00805f9b34fb ""
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002a65-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: 00002a5d-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: a026e005-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: write_request indicate
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002a66-0000-1000-8000-00805f9b34fb ""
    Capabilities: write_request indicate
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
Service: 00001826-0000-1000-8000-00805f9b34fb          ""
  Characteristic: 00002acc-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: 00002ad3-0000-1000-8000-00805f9b34fb ""
    Capabilities: read notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002ad9-0000-1000-8000-00805f9b34fb ""
    Capabilities: write_request indicate
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002ada-0000-1000-8000-00805f9b34fb ""
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002ad2-0000-1000-8000-00805f9b34fb ""
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002ad6-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: 00002ad8-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
Service: 0000181c-0000-1000-8000-00805f9b34fb          ""
  Characteristic: 00002a98-0000-1000-8000-00805f9b34fb ""
    Capabilities: read write_request
Service: a026ee0b-0a7d-4ab3-97fa-f1500f9feb8b          ""
  Characteristic: a026e037-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: read write_command notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
Service: a026ee06-0a7d-4ab3-97fa-f1500f9feb8b          ""
  Characteristic: a026e023-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: write_command notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
 */

// *interpretations* of services and characteristics based on code from GitHub repositories

/*
### Wahoo KICKR 3724 [4D250578-36AF-BE46-B680-3BF913009F67]

- Service: `0000180a-0000-1000-8000-00805f9b34fb` > "deviceInformation"
    - Characteristic: `00002a29-0000-1000-8000-00805f9b34fb` > "manufacturerNameString" [read]
    - Characteristic: `00002a25-0000-1000-8000-00805f9b34fb` > "serial_number_string" [read]
    - Characteristic: `00002a27-0000-1000-8000-00805f9b34fb` > "hardware_revision_string" [read]
    - Characteristic: `00002a26-0000-1000-8000-00805f9b34fb` > "firmwareRevisionString" [read]
- Service: `a026ee01-0a7d-4ab3-97fa-f1500f9feb8b`
- Service: `a026ee03-0a7d-4ab3-97fa-f1500f9feb8b`
- Service: `00001818-0000-1000-8000-00805f9b34fb` > "cyclingPower"
    - Characteristic: `00002a63-0000-1000-8000-00805f9b34fb` > "cyclingPowerMeasurement" [notify]
    - Characteristic: `00002a65-0000-1000-8000-00805f9b34fb` > "cyclingPowerFeature" [read]
    - Characteristic: `00002a5d-0000-1000-8000-00805f9b34fb` > "sensorLocation" [read]
    - Characteristic: `a026e005-0a7d-4ab3-97fa-f1500f9feb8b` > "wahooTrainer" [write_request indicate]
    - Characteristic: `00002a66-0000-1000-8000-00805f9b34fb` > "cyclingPowerControlPoint" [write_request indicate]
- Service: `00001826-0000-1000-8000-00805f9b34fb` > "fitnessMachine"
    - Characteristic: `00002acc-0000-1000-8000-00805f9b34fb` > "fitnessMachineFeature" [read]
    - Characteristic: `00002ad3-0000-1000-8000-00805f9b34fb` > "training_status" [read notify]
    - Characteristic: `00002ad9-0000-1000-8000-00805f9b34fb` > "fitnessMachineControlPoint" [write_request indicate]
    - Characteristic: `00002ada-0000-1000-8000-00805f9b34fb` > "fitnessMachineStatus" [notify]
    - Characteristic: `00002ad2-0000-1000-8000-00805f9b34fb` > "indoorBikeData" [notify]
    - Characteristic: `00002ad6-0000-1000-8000-00805f9b34fb` > "supportedResistanceLevelRange" [read]
    - Characteristic: `00002ad8-0000-1000-8000-00805f9b34fb` > "supportedPowerRange" [read]
- Service: `0000181c-0000-1000-8000-00805f9b34fb` > "user data"
    - Characteristic: `00002a98-0000-1000-8000-00805f9b34fb` > "weight" [read write_request]
- Service: `a026ee0b-0a7d-4ab3-97fa-f1500f9feb8b` > "wahooFitnessMachine"
    - Characteristic: `a026e037-0a7d-4ab3-97fa-f1500f9feb8b` > "wahooFitnessMachineControlPoint" [read write_command notify]
- Service: `a026ee06-0a7d-4ab3-97fa-f1500f9feb8b`
 */

// *interpretations* of services and characteristics based on ChatGPT

/*
Based on the provided attributes for a Wahoo KICKR device, here is an explanation of their meanings:

Service: 0000180a-0000-1000-8000-00805f9b34fb
This is the standard Device Information service.
Characteristic: 00002a29-0000-1000-8000-00805f9b34fb
Capabilities: read
This characteristic within the Device Information service supports the read operation. The central device can request and read the value of this characteristic, which provides device information.
Characteristic: 00002a25-0000-1000-8000-00805f9b34fb
Capabilities: read
This characteristic within the Device Information service supports the read operation. It allows the central device to read certain information about the device.
Characteristic: 00002a27-0000-1000-8000-00805f9b34fb
Capabilities: read
This characteristic within the Device Information service supports the read operation. The central device can read the value of this characteristic to obtain specific device information.
Characteristic: 00002a26-0000-1000-8000-00805f9b34fb
Capabilities: read
This characteristic within the Device Information service supports the read operation. The central device can read the value of this characteristic to get information about the firmware version of the device.
Service: a026ee01-0a7d-4ab3-97fa-f1500f9feb8b
This is a custom service provided by the Wahoo KICKR device.
Characteristic: a026e002-0a7d-4ab3-97fa-f1500f9feb8b
Capabilities: write_command notify
This characteristic supports both the write_command and notify operations. The central device can send commands or data using write commands, and the KICKR device can send notifications to the central device.
Characteristic: a026e004-0a7d-4ab3-97fa-f1500f9feb8b
Capabilities: notify
This characteristic supports the notify operation. The KICKR device can send notifications to the central device.
Characteristic: a026e03b-0a7d-4ab3-97fa-f1500f9feb8b
Capabilities: write_command notify
This characteristic supports both the write_command and notify operations. The central device can send commands or data using write commands, and the KICKR device can send notifications to the central device.
Service: 00001818-0000-1000-8000-00805f9b34fb
This is the standard Cycling Power service.
Characteristic: 00002a63-0000-1000-8000-00805f9b34fb
Capabilities: notify
This characteristic within the Cycling Power service supports the notify operation. The KICKR device can send power notifications to the central device.
Characteristic: 00002a65-0000-1000-8000-00805f9b34fb
Capabilities: read
This characteristic within the Cycling Power service supports the read operation. The central device can request and read the value of this characteristic, which provides specific power-related information.
These attributes define the services and characteristics offered by the Wahoo KICKR device, along with their corresponding capabilities such as read, write_command, and notify.
 */