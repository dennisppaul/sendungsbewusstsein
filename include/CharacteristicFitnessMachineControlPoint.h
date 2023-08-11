#pragma once

#include "Sendungsbewusstsein.h"
#include "Console.h"
#include "CharacteristicAbstract.h"
#include "CharacteristicsFactory.h"
#include "CharacteristicsGATT.h"
#include "BitUtils.h"

using namespace SimpleBLE;

/* see "4.16 Fitness Machine Control Point" p50ff from "Fitness Machine Service (v1.0)" */
/* for 'Requirements' in "Table 4.15: Fitness Machine Control Point Procedure Requirements" see "1.4 GATT Sub-Procedure Requirements" p12 */

/*

Table 1.1: GATT Sub-procedure Requirements

| GATT Sub-Procedure         | Req |
|----------------------------|-----|
| Write Characteristic Value | M   |
| Notification               | M   |
| Indication                 | C.1 |
| Read Long                  | O   |

M   = Mandatory
C.1 = Conditional
O   = Optional

*/

class CharacteristicFitnessMachineControlPoint : public CharacteristicAbstract {
public:
    CharacteristicFitnessMachineControlPoint(Peripheral *peripheral,
                                             int connected_device_index,
                                             int supported_characteristic_index)
            : CharacteristicAbstract(peripheral,
                                     connected_device_index,
                                     supported_characteristic_index) {}

    void init() override {
        subscribe();
        write({0x00}); // 0x00 = Request Control
        write({0x04, 0x08}); // 0x04 = Set Target Resistance Level to 0.8 ( = 0x08 * 0.1 )
        // write_request + indicate
    }

    void cleanup() override {
        unsubscribe();
    }

    void subscribe() override {
        console << "CharacteristicFitnessMachineControlPoint::subscribe ( > indicate )" << endl;
        auto mCallback = bind( // NOLINT(*-avoid-bind)
                &CharacteristicFitnessMachineControlPoint::indicate,
                this,
                std::placeholders::_1);
        fPeripheral->indicate(SERVICE, CHARACTERISTIC, mCallback);
    }

    void unsubscribe() override {}

    void read() override {}

    void write(SimpleBLE::ByteArray bytes) override {
        // TODO: implement

        /**
         * 4.16.2.1 Request Control Procedure
         * When the Request Control Op Code is written to the Fitness Machine Control Point and the Result Code is
         * ‘Success’, the Server shall allow the Client to perform any supported control procedures (see Sections
         * 4.16.2.2 to 4.16.2.20).
         *
         * The response shall be indicated when the Reset Procedure is completed using the Response Code Op Code and the
         * Request Op Code, along with the appropriate Result Code as defined in Section 4.16.2.22.
         *
         * The control permission remains valid until the connection is terminated, the notification of the Fitness
         * Machine Status is sent with the value set to Control Permission Lost (see Section 4.17), or the Reset
         * procedure (see Section 4.16.2.2) is initiated by the Client.
         *
         * If the operation results in an error condition where the Fitness Machine Control Point cannot be indicated
         * (e.g., the Client Characteristic Configuration descriptor is not configured for indication or if a procedure
         * is already in progress), see Section 4.16.3 for details on handling this condition.
         */
        {
//            ByteArray bytes = {0x00}; // 0x00 = Request Control
//            fPeripheral->write_request(SERVICE, CHARACTERISTIC, bytes);
            /* The response to this control point is Op Code 0x80
             * followed by the appropriate Parameter Value.
             */
        }

        /**
         * 4.16.2.5 Set Target Resistance Level Procedure
         *
         * This procedure requires control permission in order to be executed. Refer to Section 4.16.2.1
         * for more information on the Request Control procedure. When the Set Target Resistance Level
         * Op Code is written to the Fitness Machine Control Point and the Result Code is ‘Success’, the
         * Server shall set the target resistance level to the value sent as a Parameter.
         *
         * The response shall be indicated when the Set Target Resistance Level Procedure is completed
         * using the Response Code Op Code and the Request Op Code, along with the appropriate Result Code
         * as defined in Section 4.16.2.22.
         *
         * If the operation results in an error condition where the Fitness Machine Control Point cannot
         * be indicated (e.g., the Client Characteristic Configuration descriptor is not configured for
         * indication or if a procedure is already in progress), see Section 4.16.3 for details on handling
         * this condition.
         */
        {
//            ByteArray bytes = {0x04, 10}; // 0x04 = Set Target Resistance Level + parameter
//            fPeripheral->write_request(SERVICE, CHARACTERISTIC, bytes);
            /* The response to this control point is Op Code 0x80
             * followed by the appropriate Parameter Value.
             */
        }
    }

    void static register_characteristic() {
        CharacteristicFactory::register_characteristic(SERVICE, CHARACTERISTIC,
                                                       [](
                                                               Peripheral *peripheral,
                                                               int connected_device_index,
                                                               int supported_characteristic_index) -> std::unique_ptr<CharacteristicAbstract> {
                                                           return std::make_unique<CharacteristicFitnessMachineControlPoint>(
                                                                   peripheral,
                                                                   connected_device_index,
                                                                   supported_characteristic_index);
                                                       });
    }

    const char *name() override { return fName; }

private:
    constexpr static const char *fName          = "cycling_power_control_point";
    constexpr static const char *SERVICE        = SERVICE_FITNESS_MACHINE;
    constexpr static const char *CHARACTERISTIC = CHARACTERISTIC_FITNESS_MACHINE_CONTROLL_POINT_WRI;

    void indicate(ByteArray bytes) {
        // TODO e.g crank length response should be `ByteArray bytes = {0x80, 0x??, 0x01}`
        // where
        // 0x80 == Response Code
        // 0x?? == Request Op Code e.g 0x04 == Set Target Resistance Level
        // 0x01 == Result Code "Success"
        // see "Table 4.24: Fitness Machine Control Point characteristic – Result Codes" p64ff
        console << "CharacteristicFitnessMachineControlPoint::indicate" << endl;
        Utils::print_byte_array_as_bits(bytes);
        Utils::print_byte_array_as_hex(bytes);
        Utils::print_byte_array_as_dec(bytes);
        // maybe also look into "4.17 Fitness Machine Status" p66ff
    }
};
