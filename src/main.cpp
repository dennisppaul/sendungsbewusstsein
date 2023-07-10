#include <iostream>
#include <vector>
#include <chrono>
#include <string>

#include "simpleble/SimpleBLE.h"

#include "DeviceWHOOP4.h"
#include "utils.hpp"
#include "StringUtils.h"
#include "OscManager.h"
#include "HeartBeat.h"

using namespace std;
using namespace SimpleBLE;

static const int NO_DEVICE_FOUND = -1;

OscManager fOscManager;
HeartBeat  fHeartBeat(fOscManager);

void scan_for_peripherals(Adapter &adapter, std::vector<SimpleBLE::Peripheral> &peripherals, int timeout_ms) {
    adapter.set_callback_on_scan_found(
            [&](const SimpleBLE::Peripheral &peripheral) { peripherals.push_back(peripheral); });
    adapter.set_callback_on_scan_start([]() { std::cout << "Scan started." << std::endl; });
    adapter.set_callback_on_scan_stop([]() { std::cout << "Scan stopped." << std::endl; });
    adapter.scan_for(timeout_ms);

    std::cout << "The following devices were found:" << std::endl;
    for (size_t i = 0; i < peripherals.size(); i++) {
        std::cout << "[" << i << "] "
                  << peripherals[i].identifier()
                  << " [" << peripherals[i].address() << "]"
                  << std::endl;
    }
}

int find_device(std::vector<SimpleBLE::Peripheral> &peripherals, std::string &device) {
    for (int i = 0; i < peripherals.size(); i++) {
        if (peripherals[i].identifier() == device) {
            std::cout << "+++ FOUND "
                      << peripherals[i].identifier()
                      << " [" << peripherals[i].address() << "]"
                      << " [" << i << "] "
                      << std::endl;
            return i;
        }
    }
    return NO_DEVICE_FOUND;
}

int main() {
    /* --- */

//    string                 _input = "Hello World How Are, You, Today";
//    vector<std::string>    tokens = split(_input, ' ');
//    for (const std::string &t: tokens) {
//        std::cout << t << std::endl;
//    }
//
//    string                 delimiter = ", ";
//    vector<std::string>    _tokens   = split(_input, delimiter);
//    for (const std::string &t: _tokens) {
//        std::cout << t << std::endl;
//    }
//
//    fOscManager.send("test", 1.0);

    fHeartBeat.start();
    fHeartBeat.set_frequency(5000);

    /* --- */

    /* connect to adapter */
    auto adapter_optional = Utils::getAdapter();
    if (!adapter_optional.has_value()) {
        return EXIT_FAILURE;
    }
    Adapter adapter = adapter_optional.value();

    /* scan available devices */
    std::cout << "+++ SCAN FOR AVAILABLE DEVICES" << std::endl;
    std::vector<SimpleBLE::Peripheral> peripherals;
    scan_for_peripherals(adapter, peripherals, 5000);

    /* connect to desired devices */
    std::cout << "+++ CONNECT TO DEVICES" << std::endl;
    const static uint8_t NUM_OF_DEVICES           = 1;
    std::string          mDevices[NUM_OF_DEVICES] = {DeviceWHOOP4::NAME};

    int                                mPeripheralID[NUM_OF_DEVICES];
    std::vector<SimpleBLE::Peripheral> connected_peripherals;

    for (size_t i = 0; i < NUM_OF_DEVICES; i++) {
        mPeripheralID[i] = find_device(peripherals, mDevices[i]);
        if (mPeripheralID[i] != NO_DEVICE_FOUND) {
            auto peripheral = peripherals[mPeripheralID[i]];
            std::cout << "Connecting to " << peripheral.identifier() << " [" << peripheral.address() << "]"
                      << std::endl;
            peripheral.connect();
            connected_peripherals.push_back(peripheral);

//            // store all service and characteristic uuids in a vector as 'service + characteristic' pair
//            std::vector<std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuids;
//
//            for (auto service: peripheral.services()) {
//                for (auto characteristic: service.characteristics()) {
//                    uuids.emplace_back(service.uuid(), characteristic.uuid());
//                }
//            }
//            std::cout << "The following services and characteristics were found:" << std::endl;
//            for (size_t j = 0; j < uuids.size(); j++) {
//                std::cout << "[" << j << "] " << uuids[j].first << " " << uuids[j].second << std::endl;
//            }
//            // Subscribe to the characteristic.
//            int         selection = 0; /* ---------------------- */
//            peripheral.notify(uuids[selection].first, uuids[selection].second, [&](SimpleBLE::ByteArray bytes) {
//                std::cout << "Received: ";
//                Utils::print_byte_array(bytes);
//            });
//            peripheral.unsubscribe(uuids[selection.value()].first, uuids[selection.value()].second);
        }
    }

    /* iterate over connected peripherals and instantiate according classes */
    for (size_t i = 0; i < connected_peripherals.size(); i++) {
        Peripheral peripheral = connected_peripherals[i];
        if (peripheral.identifier() == string(DeviceWHOOP4::NAME)) {
            DeviceWHOOP4(i, peripheral, fOscManager);
        }
    }

//    auto selection = Utils::getUserInputInt("Please select a device to connect to", peripherals.size() - 1);
//    if (!selection.has_value()) {
//        return EXIT_FAILURE;
//    }
//
//    std::cout << "Successfully connected." << std::endl;
//    std::cout << "MTU: " << peripheral.mtu() << std::endl;

//    for (auto &service: peripheral.services()) {
//        std::cout << "Service: " << service.uuid() << std::endl;
//
//        for (auto &characteristic: service.characteristics()) {
//            std::cout << "  Characteristic: " << characteristic.uuid() << std::endl;
//
//            std::cout << "    Capabilities: ";
//            for (auto &capability: characteristic.capabilities()) {
//                std::cout << capability << " ";
//            }
//            std::cout << std::endl;
//
//            for (auto &descriptor: characteristic.descriptors()) {
//                std::cout << "    Descriptor: " << descriptor.uuid() << std::endl;
//            }
//        }
//    }

    std::cout << "+++ type 'exit', 'scan' or '?' " << std::endl;
    std::string input;
    while (std::getline(std::cin, input)) {
        /* print received command */
        if (!input.empty()) {
            std::cout << "( received command '" << input << "' )" << std::endl;

            if (input.starts_with("exit")) {
                cout << "YEAH!" << endl;
            }

            /* process input */
            if (input == "exit") {
                cout << "exiting application" << endl;
                break;
            } else if (input == "scan") {
                cout << "re-scan " << endl;
            } else {
                cout << "did not recognize command" << endl;
            }
        }

        /* prompt */
        std::cout << "> ";
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for (Peripheral peripheral: connected_peripherals) {
        peripheral.disconnect();
    }
    fOscManager.finalize();
    return EXIT_SUCCESS;
}
