#pragma once

#include <string>

#include "simpleble/SimpleBLE.h"
#include "main.h"
#include "Console.h"

using namespace std;
using namespace SimpleBLE;

class TransceiverCommandHandler {
public:
    void handle_message(string &typetag, vector<any> &message) {
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

            if (mCommand <= ERROR || mCommand >= NUM_CMDS) {
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
                    << "(" << mCommand << ") "
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

    void handle_scan_for_devices(string &typetag, vector<any> &message, const int command) {
        int mScanDurationMillis = any_cast<int>(message[1]);
        scan_for_devices(mScanDurationMillis, true);
        Transceiver::instance()->scan_for_devices(mScanDurationMillis,
                                                  get_number_of_available_devices());
    }

    void handle_connect_device(string &typetag, vector<any> &message, const int command) {
        if (typetag == COMMAND_MAP[command].typetag) {
            int mPeripheral           = any_cast<int>(message[1]);
            int mConnectedDeviceIndex = connect_device(mPeripheral);
            if (mConnectedDeviceIndex != ERROR) {
                Device    *mDevice                 = get_device(mConnectedDeviceIndex);
                const int mNumberOfCharacteristics = (mDevice != nullptr)
                                                     ? mDevice->get_number_of_supported_characteristics() : 0;
                Transceiver::instance()->connect_device(mPeripheral,
                                                        mConnectedDeviceIndex,
                                                        mNumberOfCharacteristics);
            } else {
                Transceiver::instance()->connect_device(mPeripheral,
                                                        ERROR,
                                                        0);
            }
        } else if (typetag == COMMAND_MAP[command].typetag_alt) {
            string mPeripheral           = any_cast<string>(message[1]);
            int    mConnectedDeviceIndex = connect_device(mPeripheral);
            if (mConnectedDeviceIndex != ERROR) {
                Device    *mDevice                 = get_device(mConnectedDeviceIndex);
                const int mNumberOfCharacteristics = (mDevice != nullptr)
                                                     ? mDevice->get_number_of_supported_characteristics() : 0;
                Transceiver::instance()->connect_device(mPeripheral,
                                                        mConnectedDeviceIndex,
                                                        mNumberOfCharacteristics);
            } else {
                Transceiver::instance()->connect_device(mPeripheral,
                                                        ERROR,
                                                        0);
            }
        }
    }

    void handle_command(string &typetag, vector<any> &message, const int command) {
        /*
        - `scan_for_devices`                // in main
        - `connect_device`                  // in main
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
                break;
            case CMD_SUBSCRIBE_TO_CHARACTERISTIC:
                break;
            case CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC:
                break;
            case CMD_GET_DEVICE_NAME:
                break;
            case CMD_GET_CHARACTERISTIC_NAME:
                break;
            case CMD_GET_FEATURE_NAME:
                break;
            case CMD_GET_FEATURE_VALUE:
                break;
            case CMD_SET_FEATURE_VALUE:
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