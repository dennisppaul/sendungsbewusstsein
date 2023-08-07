#pragma once

#include "SendungsbewusstseinDefines.h"
#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"
#include "BitUtils.h"
#include "utils.hpp"

using namespace SimpleBLE;

class CharacteristicCyclingPowerMeasurement : public CharacteristicAbstract {
public:
    CharacteristicCyclingPowerMeasurement(Peripheral *peripheral,
                                          int connected_device_index,
                                          int supported_characteristic_index)
            : CharacteristicAbstract(peripheral,
                                     connected_device_index,
                                     supported_characteristic_index) {}

    void init() override {
        subscribe();
    }

    void subscribe() override {
        console
                << "subscribing to '"
                << name()
                << "' "
                << "as service "
                << SERVICE
                << " with characteristic "
                << CHARACTERISTIC
                << std::endl;

        auto mCallback = bind( // NOLINT(*-avoid-bind)
                &CharacteristicCyclingPowerMeasurement::notify,
                this,
                std::placeholders::_1);
        fPeripheral->notify(SERVICE, CHARACTERISTIC, mCallback);
        Transceiver::instance()->send_characteristic_command(fConnectedDeviceIndex,
                                                             CMD_RESPONSE_SUBSCRIBED,
                                                             fName,
                                                             fSupportedCharacteristicIndex);
    }

    void unsubscribe() override { fPeripheral->unsubscribe(SERVICE, CHARACTERISTIC); }

    void read() override {}

    void write(SimpleBLE::ByteArray bytes) override {}

    const char *name() override { return fName; }

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE, CHARACTERISTIC,
                                                       [](
                                                               Peripheral *peripheral,
                                                               int connected_device_index,
                                                               int supported_characteristic_index) -> std::unique_ptr<CharacteristicAbstract> {
                                                           return std::make_unique<CharacteristicCyclingPowerMeasurement>(
                                                                   peripheral,
                                                                   connected_device_index,
                                                                   supported_characteristic_index);
                                                       });
    }

private:
    constexpr static const char *fName                             = "cycling_power_measurement";
    constexpr static const char *SERVICE                           = SERVICE_CYCLING_POWER;
    constexpr static const char *CHARACTERISTIC                    = CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N;
    constexpr static const char *FEATURE_STR_POWER                 = "power";
    constexpr static const char *FEATURE_STR_ACCUMULATED_TORQUE    = "accumulated_torque";
    constexpr static const char *FEATURE_STR_WHEEL_REVOLUTION_DATA = "wheel_revolutions";
    constexpr static const char *FEATURE_STR_CRANK_REVOLUTION_DATA = "crank_revolutions";

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
    */
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

    static void print_features(uint16_t flags) {
        if (flags & FLAG_PEDAL_POWER_BALANCE_PRESENT) {
            console << "FLAG_PEDAL_POWER_BALANCE_PRESENT" << endl;
            console << "    FLAG_PEDAL_POWER_BALANCE_REFERENCE: ";
            if (flags & FLAG_PEDAL_POWER_BALANCE_REFERENCE_LEFT) {
                console << "LEFT";
            } else {
                console << "UNKNOWN";
            }
            console << endl;
        }
        if (flags & FLAG_ACCUMULATED_TORQUE_PRESENT) {
            console << "FLAG_ACCUMULATED_TORQUE_PRESENT" << endl;
            console << "    FLAG_ACCUMULATED_TORQUE_SOURCE_CRANK_BASED: ";
            if (flags & FLAG_ACCUMULATED_TORQUE_SOURCE_CRANK_BASED) {
                console << "CRANK";
            } else {
                console << "WHEEL";
            }
            console << endl;
        }
        if (flags & FLAG_WHEEL_REVOLUTION_DATA_PRESENT) {
            console << "FLAG_WHEEL_REVOLUTION_DATA_PRESENT" << endl;
        }
        if (flags & FLAG_CRANK_REVOLUTION_DATA_PRESENT) {
            console << "FLAG_CRANK_REVOLUTION_DATA_PRESENT" << endl;
        }
        if (flags & FLAG_EXTREME_FORCE_MAGNITUDES_PRESENT) {
            console << "FLAG_EXTREME_FORCE_MAGNITUDES_PRESENT" << endl;
        }
        if (flags & FLAG_EXTREME_TORQUE_MAGNITUDES_PRESENT) {
            console << "FLAG_EXTREME_TORQUE_MAGNITUDES_PRESENT" << endl;
        }
        if (flags & FLAG_EXTREME_ANGLES_PRESENT) {
            console << "FLAG_EXTREME_ANGLES_PRESENT" << endl;
        }
        if (flags & FLAG_TOP_DEAD_SPOT_ANGLE_PRESENT) {
            console << "FLAG_TOP_DEAD_SPOT_ANGLE_PRESENT" << endl;
        }
        if (flags & FLAG_BOTTOM_DEAD_SPOT_ANGLE_PRESENT) {
            console << "FLAG_BOTTOM_DEAD_SPOT_ANGLE_PRESENT" << endl;
        }
        if (flags & FLAG_ACCUMULATED_ENERGY_PRESENT) {
            console << "FLAG_ACCUMULATED_ENERGY_PRESENT" << endl;
        }

        // TODO is this related to presence of another feature?
        console << "FLAG_OFFSET_COMPENSATION_INDICATOR: ";
        if (flags & FLAG_OFFSET_COMPENSATION_INDICATOR) {
            console << "TRUE";
        } else {
            console << "FALSE";
        }
        console << endl;
    }

    void notify(ByteArray bytes) {
        uint16_t flags = bytes_to_uint16(bytes[1], bytes[0]);
#ifdef DEBUG_CYCLING_POWER_MEASUREMENT_FEATURES
        console << "CyclingPowerMeasurement:" << endl;
        Utils::print_byte_array_as_bits(bytes);
        print_features(flags);
#endif // DEBUG_CYCLING_POWER_MEASUREMENT_FEATURES

        /*
        ### Table: Parameters ( 3.65 Cycling Power Measurement, Table 3.112: Structure of the Cycling Power Measurement characteristic from 'GATT_Specification_Supplement_v10'

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

        // example from 'Wahoo KICKR':
        // 00101100 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
        // |- FLAGS -------| |- POWER -------| |- TORQUE ------| |- WHEEL REVOLUTIONS -------------------------------| |- CRANKREVOLUTION ---------------|

        const auto mInstantaneousPower = (float) bytes_to_uint16(bytes[3], bytes[2]); /* power is always present */
        //         | Instantaneous Power       | sint16       | 2                | unit: watts             |
        Transceiver::instance()->send_characteristic_feature_with_value(fConnectedDeviceIndex,
                                                                        fName,
                                                                        FEATURE_STR_POWER,
                                                                        mInstantaneousPower);

        int i = 4;
        if (flags & FLAG_PEDAL_POWER_BALANCE_PRESENT) {
            //         | Pedal Power Balance       | uint8        | 0 or 1           | unit: 1/2 percent       |
            i += 1;
        }
        if (flags & FLAG_ACCUMULATED_TORQUE_PRESENT) {
            //         | Accumulated Torque        | uint16       | 0 or 2           | unit: 1/32 Newton meter |
            const auto mAccumulatedTorque = (float) bytes_to_uint16(bytes[i + 1], bytes[i]);
            Transceiver::instance()->send_characteristic_feature_with_value(fConnectedDeviceIndex,
                                                                            fName,
                                                                            FEATURE_STR_ACCUMULATED_TORQUE,
                                                                            mAccumulatedTorque);
            i += 2;
        }
        if (flags & FLAG_WHEEL_REVOLUTION_DATA_PRESENT) {
            //         | Wheel Revolution Data     | struct       | 0 or 6           |                         |
            // see '3.65.2 Wheel Revolution Data field'
            // > Cumulative Wheel Revolutions :: uint32
            // > Last Wheel Event Time        :: uint16
            const auto mCumulativeWheelRevolutions = (float) bytes_to_uint32(bytes[i + 3],
                                                                             bytes[i + 2],
                                                                             bytes[i + 1],
                                                                             bytes[i]);
            Transceiver::instance()->send_characteristic_feature_with_value(fConnectedDeviceIndex,
                                                                            fName,
                                                                            FEATURE_STR_WHEEL_REVOLUTION_DATA,
                                                                            mCumulativeWheelRevolutions);
            i += 6;
        }
        if (flags & FLAG_CRANK_REVOLUTION_DATA_PRESENT) {
            //         | Crank Revolution Data     | struct       | 0 or 4           |                         |
            // see '3.65.3 Crank Revolution Data field'
            // > Cumulative Crank Revolutions :: uint16
            // > Last Crank Event Time        :: uint16
            const auto mCumulativeCrankRevolutions = (float) bytes_to_uint16(bytes[i + 1],
                                                                             bytes[i]);
            Transceiver::instance()->send_characteristic_feature_with_value(fConnectedDeviceIndex,
                                                                            fName,
                                                                            FEATURE_STR_CRANK_REVOLUTION_DATA,
                                                                            mCumulativeCrankRevolutions);
            i += 4;
        }
        if (flags & FLAG_EXTREME_FORCE_MAGNITUDES_PRESENT) {
            //         | Extreme Force Magnitudes  | struct       | 0 or 4           |                         |
            i += 4;
        }
        if (flags & FLAG_EXTREME_TORQUE_MAGNITUDES_PRESENT) {
            //         | Extreme Torque Magnitudes | struct       | 0 or 4           |                         |
            i += 4;
        }
        if (flags & FLAG_EXTREME_ANGLES_PRESENT) {
            //         | Extreme Angles            | struct       | 0 or 3           |                         |
            i += 3;
        }
        if (flags & FLAG_TOP_DEAD_SPOT_ANGLE_PRESENT) {
            //         | Top Dead Spot Angle       | uint16       | 0 or 2           | unit: angle degree      |
            i += 2;
        }
        if (flags & FLAG_BOTTOM_DEAD_SPOT_ANGLE_PRESENT) {
            //         | Bottom Dead Spot Angle    | uint16       | 0 or 2           | unit: angle degree      |
            i += 2;
        }
        if (flags & FLAG_ACCUMULATED_ENERGY_PRESENT) {
            //         | Accumulated Energy        | uint16       | 0 or 2           | unit: kilojoule         |
        }
    }
};
