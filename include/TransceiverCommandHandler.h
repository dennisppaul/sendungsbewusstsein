#pragma once

#include <string>

#include "simpleble/SimpleBLE.h"
#include "main.h"
#include "Console.h"

using namespace std;
using namespace SimpleBLE;

class TransceiverCommandHandler {
public:
    static void handle_message(string &typetag, vector<any> &message) {
        if (message.empty()) {
            error
                    << "message is empty"
                    << endl;
            return;
        }

        if (typetag.starts_with("i")) {
            int mCommand = ERROR;
            if (message.front().type() == typeid(int)) {
                mCommand = std::any_cast<int>(message[0]);
            }

            if (mCommand <= ERROR || mCommand >= NUM_COMMANDS) {
                error
                        << "command ("
                        << mCommand
                        << ") is not supported."
                        << endl;
                return;
            }

            bool mIsCommandFormattedOK = evaluate_transceiver_command(typetag, mCommand);
            if (!mIsCommandFormattedOK) {
                error
                        << "command '"
                        << string(COMMAND_MAP[mCommand].name)
                        << "'"
                        << "(" << mCommand << ") "
                        << "is supported but message typetag does not match."
                        << "expected: '"
                        << COMMAND_MAP[mCommand].typetag
                        << (COMMAND_MAP[mCommand].typetag_alt != nullptr ? "' or '" : "")
                        << (COMMAND_MAP[mCommand].typetag_alt != nullptr ? COMMAND_MAP[mCommand].typetag_alt
                                                                         : "")
                        << "' but found "
                        << typetag
                        << endl;
                return;
            }

            console
                    << "command '"
                    << string(COMMAND_MAP[mCommand].name)
                    << "'"
                    << " (" << mCommand << ") "
                    << "is supported."
                    << endl;
            handle_command(typetag, message, mCommand);
            return;
        }

        console
                << "ignoring message: "
                << typetag
                << " (" << message.size()
                << ")"
                << endl;
    }

private:

    static void die(string &typetag, vector<any> &message, const int command) {
        error
                << "problem parsing connect message: "
                << command
                << " ("
                << typetag
                << ")"
                << "["
                << message.size()
                << "]"
                << endl;
    }

    static bool evaluate_transceiver_command(string &typetag, const int command) {
        for (auto &mCommand: COMMAND_MAP) {
            if (mCommand.cmd == command) {
                if (equals(typetag, mCommand.typetag)) {
                    return true;
                }
                if (mCommand.typetag_alt != nullptr) {
                    if (equals(typetag, mCommand.typetag_alt)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    static void handle_scan_for_devices(string &typetag, vector<any> &message, const int command) {
//        #### scan_for_devices
//
//        client requests to update available device list.
//
//        ```
//        command .... : scan_for_devices(duration_in_milliseconds)
//        typetag .... : ii
//        example .... : CMD_SCAN_FOR_DEVICES,5000
//
//        response ... : command,
//                duration_in_milliseconds,
//                number_of_devices
//        typetag .... : iii
//        example .... : CMD_SCAN_FOR_DEVICES,5000,12
//        ```

        if (message[1].type() == typeid(int)) {
            int mScanDurationMillis = any_cast<int>(message[1]);
            scan_for_devices(mScanDurationMillis, true);
            Transceiver::instance()->scan_for_devices(mScanDurationMillis,
                                                      get_number_of_available_devices());
        } else {
            die(typetag, message, command);
        }
    }

    static void handle_connect_device(string &typetag, vector<any> &message, const int command) {
//        #### connect_device
//
//        client requests that the server connects to a device. note, that a device can be specified either by *available* device index, name or UUID. further note, that the *available* device index refers to the order of list of scanned devices, whereas the *‌device_index* which is returned in the response refers to the index of the connected devices.
//
//        ```
//        command .... : connect_device(available_device_index/name/UUID)
//        typetag .... : i(i/s/s)
//        example .... : CMD_CONNECT_DEVICE,"Wahoo KICKR"
//
//        response ... : command,
//                available_device_index/name/UUID,
//                device_index,
//                number_of_characteristics
//        typetag .... : i(i/s/s)ii
//        example .... : CMD_CONNECT_DEVICE,"Wahoo KICKR",0,4
//        ```

        if (typetag == COMMAND_MAP[command].typetag) {
            if (message[1].type() == typeid(int)) {
                int mPeripheral           = any_cast<int>(message[1]);
                int mConnectedDeviceIndex = connect_device(mPeripheral);
                if (mConnectedDeviceIndex != ERROR) {
                    const shared_ptr<Device> &mDevice                 = get_device(mConnectedDeviceIndex);
                    const int                mNumberOfCharacteristics = (mDevice != nullptr)
                                                                        ? mDevice->get_number_of_supported_characteristics()
                                                                        : 0;
                    Transceiver::instance()->connect_device(mPeripheral,
                                                            mConnectedDeviceIndex,
                                                            mNumberOfCharacteristics);
                } else {
                    Transceiver::instance()->connect_device(mPeripheral,
                                                            ERROR,
                                                            0);
                }
                return;
            }
        } else if (typetag == COMMAND_MAP[command].typetag_alt) {
            if (message[1].type() == typeid(const char *) || message[1].type() == typeid(std::string)) {
                auto mPeripheral           = string(any_cast<const char *>(message[1]));
                int  mConnectedDeviceIndex = connect_device(mPeripheral);
                if (mConnectedDeviceIndex != ERROR) {
                    const shared_ptr<Device> &mDevice                 = get_device(mConnectedDeviceIndex);
                    const int                mNumberOfCharacteristics = (mDevice != nullptr)
                                                                        ? mDevice->get_number_of_supported_characteristics()
                                                                        : 0;
                    Transceiver::instance()->connect_device(mPeripheral,
                                                            mConnectedDeviceIndex,
                                                            mNumberOfCharacteristics);
                } else {
                    Transceiver::instance()->connect_device(mPeripheral,
                                                            ERROR,
                                                            0);
                }
                return;
            }
        }
        die(typetag, message, command);
    }

    static void handle_disconnect_device(string &typetag, vector<any> &message, const int command) {
//        #### disconnect_device
//
//        client requests that the server disconnects from a device.
//
//        ```
//        command .... : disconnect_device(device_index)
//        typetag .... : ii
//        example .... : CMD_DISCONNECT_DEVICE,0
//
//        response ... : command,
//                       device_index
//        typetag .... : ii
//        example .... : CMD_DISCONNECT_DEVICE,0
//        ```
        if (message[1].type() == typeid(int)) {
            int mDeviceIndex = any_cast<int>(message[1]);
            if (mDeviceIndex == ALL_DEVICES) {
                for (int i = 0; i < get_number_of_connected_devices(); i++) {
                    disconnect_device(i);
                    Transceiver::instance()->disconnect_device(i);
                }
                reset_connected_devices();
            } else {
                disconnect_device(mDeviceIndex);
                Transceiver::instance()->disconnect_device(mDeviceIndex);
            }
        } else {
            die(typetag, message, command);
        }
    }

    static void handle_set_feature_value(string &typetag, vector<any> &message, const int command) {
//        #### set_feature_value
//
//        client requests to set the value of a feature within a characteristic.
//
//        ```
//        command .... : set_feature_value(device_index,
//                                         characteristic_index,
//                                         (feature_index/feature_name),
//                                         value)
//        typetag .... : iii(i/s)f
//        example .... : CMD_SET_FEATURE_VALUE,0,1,123
//
//        response ... : command,
//                       device_index,
//                       characteristic_index,
//                       feature_name,
//                       feature_value
//        typetag .... : iii(i/s)f
//        example .... : CMD_SET_FEATURE_VALUE,0,1,"heartrate",123
//        ```
        int   device_index;
        int   characteristic_index;
        int   feature_index;
        float feature_value;
        if (!(typetag == COMMAND_MAP[command].typetag || typetag == COMMAND_MAP[command].typetag_alt)) {
            die(typetag, message, command);
        }
        device_index         = any_cast<int>(message[1]);
        characteristic_index = any_cast<int>(message[2]);
        feature_value        = any_cast<float>(message[4]);

        shared_ptr<Device> mDevice = get_device(device_index);
        if (mDevice == get_null_device()) {
            error
                    << "device with index '"
                    << device_index
                    << "' does not exist."
                    << endl;
            die(typetag, message, command);
            return;
        }
        if (!(characteristic_index >= 0 && characteristic_index < mDevice->get_number_of_supported_characteristics())) {
            error
                    << "device with index '"
                    << device_index
                    << "' does not have characteristic with index '"
                    << characteristic_index
                    << "'"
                    << endl;
            die(typetag, message, command);
            return;
        }
        CharacteristicAbstract *mCharacteristic = mDevice->get_characteristic(characteristic_index);
        if (mCharacteristic == nullptr) {
            error
                    << "device with index '"
                    << device_index
                    << "' has not intialized characteristic with index '"
                    << characteristic_index
                    << "'"
                    << endl;
            die(typetag, message, command);
            return;
        }

        if (typetag == COMMAND_MAP[command].typetag_alt) {
            auto feature_name = std::string(any_cast<const char *>(message[3]));
            feature_index = mCharacteristic->get_feature_index_from_string(feature_name);
            if (!(feature_index >= 0 && feature_index < mCharacteristic->get_number_of_supported_features())) {
                error
                        << "device with index '"
                        << device_index
                        << "' and characteristic '"
                        << characteristic_index
                        << "' does not have feature with name '"
                        << feature_name
                        << "'"
                        << endl;
                mCharacteristic->print_features();
                die(typetag, message, command);
                return;
            }
        } else {
            feature_index = any_cast<int>(message[3]);
            if (!(feature_index >= 0 && feature_index < mCharacteristic->get_number_of_supported_features())) {
                error
                        << "device with index '"
                        << device_index
                        << "' and characteristic '"
                        << characteristic_index
                        << "' does not have feature with index '"
                        << feature_index
                        << "'"
                        << endl;
                mCharacteristic->print_features();
                die(typetag, message, command);
                return;
            }
        }

        Feature *mFeature = mCharacteristic->get_feature(feature_index);
        if (mFeature == nullptr) {
            error
                    << "device with index '"
                    << device_index
                    << "' and characteristic '"
                    << characteristic_index
                    << "' has not intialized feature with index '"
                    << feature_index
                    << "'"
                    << endl;
            die(typetag, message, command);
            return;
        }

        mFeature->invoke_set_value(feature_value);
        // TODO sent error messages as well … see above
        if (typetag == COMMAND_MAP[command].typetag_alt) {
            auto feature_name = std::string(any_cast<const char *>(message[3]));
            Transceiver::instance()->set_feature_value(device_index,
                                                       characteristic_index,
                                                       feature_name,
                                                       feature_value);
        } else {
            Transceiver::instance()->set_feature_value(device_index,
                                                       characteristic_index,
                                                       feature_index,
                                                       feature_value);
        }
    }

    static void handle_command(string &typetag, vector<any> &message, const int command) {
        /*
         * TODO
        - `disconnect_device`               // in main
        - `subscribe_to_characteristic`     // in Device
        - `unsubscribe_from_characteristic` // in Device
        - `get_device_name`                 // in Devive
        - `get_characteristic_name`         // in Device
        - `get_feature_name`                // in Characteristic
        - `get_feature_value`               // in Characteristic
        - `set_feature_value`               // in Characteristic
         */
        switch (command) {
            case CMD_SCAN_FOR_DEVICES:
                handle_scan_for_devices(typetag, message, command);
                break;
            case CMD_CONNECT_DEVICE:
                handle_connect_device(typetag, message, command);
                break;
            case CMD_DISCONNECT_DEVICE:
                handle_disconnect_device(typetag, message, command);
                break;
            case CMD_SUBSCRIBE_TO_CHARACTERISTIC:
                console
                        << "CMD_SUBSCRIBE_TO_CHARACTERISTIC not implemented"
                        << endl;
                // TODO maybe send all device features, too?
                break;
            case CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC:
                console
                        << "CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC not implemented"
                        << endl;
                break;
            case CMD_GET_DEVICE_NAME:
                console
                        << "CMD_GET_DEVICE_NAME not implemented"
                        << endl;
                break;
            case CMD_GET_CHARACTERISTIC_NAME:
                console
                        << "CMD_GET_CHARACTERISTIC_NAME not implemented"
                        << endl;
                break;
            case CMD_GET_FEATURE_NAME:
                console
                        << "CMD_GET_FEATURE_NAME not implemented"
                        << endl;
                break;
            case CMD_GET_FEATURE_VALUE:
                console
                        << "CMD_GET_FEATURE_VALUE not implemented"
                        << endl;
                break;
            case CMD_SET_FEATURE_VALUE:
                handle_set_feature_value(typetag, message, command);
                break;
            default:
                error << "unknown command: " << command << endl;
        }
        //    for (const auto &item: message) {
        //        if (item.type() == typeid(int)) {
        //            std::cout << "Found an integer: " << std::any_cast<int>(item) << std::endl;
        //        } else if (item.type() == typeid(float)) {
        //            std::cout << "Found a float: " << std::any_cast<float>(item) << std::endl;
        //        } else if (item.type() == typeid(const char *) || item.type() == typeid(std::string)) {
        //            std::cout << "Found a string: " << std::any_cast<const char *>(item) << std::endl;
        //        } else {
        //            std::cout << "Unknown type" << std::endl;
        //        }
        //    }
    }
};