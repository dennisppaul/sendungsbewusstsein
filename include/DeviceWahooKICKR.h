#pragma once

#include "Device.h"
#include "BitUtils.h"

class DeviceWahooKICKR : public Device {
public:
//    static constexpr const char *NAME = "Wahoo KICKR";
//    static constexpr const char *NAME = "Wahoo KICKR 3724";

    DeviceWahooKICKR(int osc_index, Peripheral &peripheral) :
            Device("Wahoo KICKR", osc_index),
            fPeripheral(peripheral) {
        connect_impl();
        subscribe_indoor_bike_data();
//        subscribe_cycling_power_measurement();
    }

    DeviceWahooKICKR(const DeviceWahooKICKR &) = delete;

    ~DeviceWahooKICKR() override = default;

    void connect() override { connect_impl(); }

    void disconnect() override {
        fPeripheral.disconnect();
        OscSenderReceiver::instance()->send(name(), CMD_DISCONNECT, ID());
    }

private:
    Peripheral                                 &fPeripheral;
    vector<pair<BluetoothUUID, BluetoothUUID>> uuids;

    void update_services() {
        for (auto service: fPeripheral.services()) {
            for (auto characteristic: service.characteristics()) {
                uuids.emplace_back(service.uuid(), characteristic.uuid());
            }
        }
        for (auto &service: fPeripheral.services()) {
            console << "Service: " << service.uuid() << endl;

            for (auto &characteristic: service.characteristics()) {
                console << "  Characteristic: " << characteristic.uuid() << endl;

                console << "    Capabilities: ";
                for (auto &capability: characteristic.capabilities()) {
                    console << capability << " ";
                }
                console << endl;

                for (auto &descriptor: characteristic.descriptors()) {
                    console << "    Descriptor: " << descriptor.uuid() << endl;
                }
            }
        }
    }

    void connect_impl() {
        fPeripheral.connect();
        update_services();
        OscSenderReceiver::instance()->send(name(), CMD_CONNECT, ID());

        console << "+++ reading CHARACTERISTIC_FITNESS_MACHINE_FEATURE_R: ";
        Utils::print_byte_array_as_bits(
                fPeripheral.read(SERVICE_FITNESS_MACHINE, CHARACTERISTIC_FITNESS_MACHINE_FEATURE_R));
        Utils::print_byte_array(fPeripheral.read(SERVICE_FITNESS_MACHINE, CHARACTERISTIC_FITNESS_MACHINE_FEATURE_R));
        console << "+++ reading CHARACTERISTIC_CYCLING_POWER_FEATURE_R  : ";
        Utils::print_byte_array_as_bits(
                fPeripheral.read(SERVICE_CYCLING_POWER, CHARACTERISTIC_CYCLING_POWER_FEATURE_R));
        Utils::print_byte_array(fPeripheral.read(SERVICE_CYCLING_POWER, CHARACTERISTIC_CYCLING_POWER_FEATURE_R));
        console << endl;
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


    void notify_indoor_bike_data(ByteArray bytes) {
        // Instantaneous Speed ( 0.01 km/h )
        float speed   = (float) bytes_to_uint16(bytes[3], bytes[2]) * 0.01f;
        // Instantaneous Cadence ( 0.5 /min )
        float cadence = (float) bytes_to_uint16(bytes[5], bytes[4]) * 0.5f;
        // Instantaneous Power ( 1 watt )
        float power   = bytes_to_int16(bytes[7], bytes[6]);
        OscSenderReceiver::instance()->send(name(), "speed", ID(), speed);
        OscSenderReceiver::instance()->send(name(), "power", ID(), power);
        OscSenderReceiver::instance()->send(name(), "cadence", ID(), cadence);

//#define  WAHOO_KICKR_DEBUG_BIKE_DATA
#ifdef  WAHOO_KICKR_DEBUG_BIKE_DATA
        console << "NOTIFICATION: ";
        Utils::print_byte_array(bytes);
        console << "FLAG        : ";
        Utils::print_byte(bytes[0]);
        console << " ";
        Utils::print_byte(bytes[1]);
        console << endl;
        console << "indoor bike data /"
                << " speed: " << speed
                << " cadence: " << cadence
                << " power: " << power
                << endl;
        /*
        | Bit | Flag                  |
        |-----|-----------------------|
        | 0   | More Data             |
        | 1   | Average Speed         |
        | 2   | Instantaneous Cadence |
        | 3   | Average Cadence       |
        | 4   | Total Distance        |
        | 5   | Resistance Level      |
        | 6   | Instantaneous Power   |
        | 7   | Average Power         |
        | 8   | Expended Energy       |
        | 9   | Heart Rate            |
        | 10  | Metabolic Equivalent  |
        | 11  | Elapsed Time          |
        | 12  | Remaining Time        |
        | 13  | UNUSED                |
        | 14  | UNUSED                |
        | 15  | UNUSED                |

        | Parameter             | Format | Unit       |
        |-----------------------|--------|------------|
        | Instantaneous Speed   | uint16 | 0.01 Km/h  |
        | Average Speed         | uint16 | 0.01 Km/h  |
        | Instantaneous Cadence | uint16 | 0.5 /min   |
        | Average Cadence       | uint16 | 0.5 /min   |
        | Total Distance        | uint24 | metre      |
        | Resistance Level      | unit8  | 1 unitless |
        | Instantaneous Power   | sint16 | 1 watt     |
        | Average Power         | sint16 | 1 watt     |
        | Total Energy          | uint16 | kcal       |
        | Energy Per Hour       | uint16 | kcal       |
        | Energy Per Minute     | uint8  | kcal       |
        | Heart Rate            | uint8  | Beats/m    |
        | Metabolic Equivalent  | uint8  | 0.1 meta   |
        | Elapsed Time          | uint16 | second     |
        | Remaining Time        | uint16 | second     |
        */

        /*
        00100010 // Instantaneous Speed field present ( flag = 0 ), Instantaneous Cadence present, Instantaneous Power present
        00000000 //
         */
#endif
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


    void notify_cycling_power_measurement(ByteArray bytes) {
        enum {
            FLAG_PEDAL_POWER_BALANCE_PRESENT           = 0x0001,
            FLAG_PEDAL_POWER_BALANCE_REFERENCE_LEFT    = 0x0002,
            FLAG_ACCUMULATED_TORQUE_PRESENT            = 0x0004,
            FLAG_ACCUMULATED_TORQUE_SOURCE_CRANK_BASED = 0x0008,
            FLAG_WHEEL_REVOLUTION_DATA_PRESENT         = 0x0010,
            FLAG_CRANK_REVOLUTION_DATA_PRESENT         = 0x0020,
            FLAG_EXTREME_FORCE_MAGNITUDES_PRESENT      = 0x0040,
            FLAG_EXTREME_TORQUE_MAGNITUDES_PRESENT     = 0x0080,
            FLAG_EXTREME_ANGLES_PRESENT                = 0x0100,
            FLAG_TOP_DEAD_SPOT_ANGLE_PRESENT           = 0x0200,
            FLAG_BOTTOM_DEAD_SPOT_ANGLE_PRESENT        = 0x0400,
            FLAG_ACCUMULATED_ENERGY_PRESENT            = 0x0800,
            FLAG_OFFSET_COMPENSATION_INDICATOR         = 0x1000,
        };

        enum {
            FEATURE_PEDAL_POWER_BALANCE                 = 0x00000001,
            FEATURE_ACCUMULATED_TORQUE                  = 0x00000002,
            FEATURE_WHEEL_REVOLUTION_DATA               = 0x00000004,
            FEATURE_CRANK_REVOLUTION_DATA               = 0x00000008,
            FEATURE_EXTREME_MAGNITUDES                  = 0x00000010,
            FEATURE_EXTREME_ANGLES                      = 0x00000020,
            FEATURE_TOP_AND_BOTTOM_DEAD_SPOT_ANGLES     = 0x00000040,
            FEATURE_ACCUMULATED_ENERGY                  = 0x00000080,
            FEATURE_OFFSET_COMPENSATION_INDICATOR       = 0x00000100,
            FEATURE_OFFSET_COMPENSATION                 = 0x00000200,
            FEATURE_MEASUREMENT_CONTENT_MASKING         = 0x00000400,
            FEATURE_MULTIPLE_SENSOR_LOCATIONS           = 0x00000800,
            FEATURE_CRANK_LENGTH_ADJUSTMENT             = 0x00001000,
            FEATURE_CHAIN_LENGTH_ADJUSTMENT             = 0x00002000,
            FEATURE_CHAIN_WEIGHT_ADJUSTMENT             = 0x00004000,
            FEATURE_SPAN_LENGTH_ADJUSTMENT              = 0x00008000,
            FEATURE_SENSOR_MEASUREMENT_TORQUE_BASED     = 0x00010000,
            FEATURE_INSTANTANEOUS_MEASUREMENT_DIRECTION = 0x00020000,
            FEATURE_FACTORY_CALIBRATION_DATE            = 0x00040000,
        };

        uint16_t flags = bytes_to_uint16(bytes[1], bytes[0]);
        uint16_t power = bytes_to_uint16(bytes[3], bytes[2]);
        console << dec << "power: " << power << endl;
//        int i = 4;
        if (flags & FLAG_WHEEL_REVOLUTION_DATA_PRESENT) {
//            measureData[i++] = (uint8_t)(cumulativeWheelRev & 0xFF);
//            measureData[i++] = (uint8_t)((cumulativeWheelRev >>  8) & 0xFF);
//            measureData[i++] = (uint8_t)((cumulativeWheelRev >> 16) & 0xFF);
//            measureData[i++] = (uint8_t)((cumulativeWheelRev >> 24) & 0xFF);
//
//            measureData[i++] = (uint8_t)(lastWheelEventTime & 0xFF);
//            measureData[i++] = (uint8_t)(lastWheelEventTime >> 8);
        }
        OscSenderReceiver::instance()->send(name(), "power", ID(), power);

        /*
        ### Table: Flags ( 3.65.1 Flags field, Table 3.113: Flags field )

        | Bit   | Definition                                                     |
        |-------|----------------------------------------------------------------|
        | 0     | Pedal Power Balance Present                                    |
        | 1     | Pedal Power Balance Reference ( 0 = Unknown, 1 = Left )        |
        | 2     | Accumulated Torque Present                                     |
        | 3     | Accumulated Torque Source ( 0 = Wheel based, 1 = Crank based ) |
        | 4     | Wheel Revolution Data Present                                  |
        | 5     | Crank Revolution Data Present                                  |
        | 6     | Extreme Force Magnitudes Present                               |
        | 7     | Extreme Torque Magnitudes Present                              |
        | 8     | Extreme Angles Present                                         |
        | 9     | Top Dead Spot Angle Present                                    |
        | 10    | Bottom Dead Spot Angle Present                                 |
        | 11    | Accumulated Energy Present                                     |
        | 12    | Offset Compensation Indicator ( 0 = False, 1 = True )          |
        | 13–15 | Reserved for Future Use (RFU)                                  |

        ### Table: Parameters ( 3.65 Cycling Power Measurement, Table 3.112: Structure of the Cycling Power Measurement characteristic )

        | Field                     | Data Type    | Size (in octets) | Description             |
        |---------------------------|--------------|------------------|-------------------------|
        | Flags                     | boolean[16]  | 2                |                         |
        | Instantaneous Power       | sint16       | 2                | unit: watts             |
        | Pedal Power Balance       | uint8        | 0 or 1           | unit: 1/2 percent       |
        | Accumulated Torque        | uint16       | 0 or 2           | unit: 1/32 Newton meter |
        | Wheel Revolution Data     | struct       | 0 or 6           |                         |
        | Crank Revolution Data     | struct       | 0 or 4           |                         |
        | Extreme Force Magnitudes  | struct       | 0 or 4           |                         |
        | Extreme Torque Magnitudes | struct       | 0 or 4           |                         |
        | Extreme Angles            | struct       | 0 or 3           |                         |
        | Top Dead Spot Angle       | uint16       | 0 or 2           | unit: angle degree      |
        | Bottom Dead Spot Angle    | uint16       | 0 or 2           | unit: angle degree      |
        | Accumulated Energy        | uint16       | 0 or 2           | unit: kilojoule         |
         */

        // TODO this is just a quick dirty hack
        // TODO is the byte order reversed?
//        uint16_t flags                        = (bytes[0] << 8) | bytes[1];
//        auto instantaneous_power = static_cast<int16_t>((static_cast<uint16_t>(bytes[3]) << 8) | (bytes[2]));
//        int16_t instantaneous_power = ((int16_t) bytes[3] * 256) + (uint8_t) bytes[2];
//        auto instantaneous_power = static_cast<int16_t>((static_cast<int16_t>(bytes[3]) * 256) +
//                                                        static_cast<uint8_t>(bytes[2]));
//        uint16_t accumulated_torque           = (bytes[4] << 8) | bytes[5];
//        uint32_t cumulative_wheel_revolutions = (bytes[6] << 24) | (bytes[7] << 16) | (bytes[8] << 8) | bytes[9];
//        uint16_t last_wheel_event_time        = (bytes[10] << 8) | bytes[11];
//        uint16_t cumulative_crank_revolutions = (bytes[12] << 8) | bytes[13];
//        uint16_t last_crank_event_time        = (bytes[14] << 8) | bytes[15];
//#define  WAHOO_KICKR_DEBUG_PWR
//#ifdef  WAHOO_KICKR_DEBUG_PWR
//        console << dec;
//        console << "instantaneous_power: " << instantaneous_power;
//        console << " \t ";
//        console << "accumulated_torque: " << accumulated_torque;
//        console << " \t ";
//        console << "cumulative_wheel_revolutions: " << cumulative_wheel_revolutions;
//        console << " \t ";
//        console << "cumulative_crank_revolutions: " << cumulative_crank_revolutions;
//        console << endl;
//
//        const float mHeartRate = bytes[1];
//        console << "PWR: ";
//        console << fixed << setprecision(0) << mHeartRate << " / ";
//        Utils::print_byte_array(bytes);
//#endif
//        OscSenderReceiver::instance()->send(name(), fID, feature_cycling_power_measurement.description, instantaneous_power);
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