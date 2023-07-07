#include <iostream>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include "simpleble/SimpleBLE.h"

#include "utils.hpp"
#include "OscManager.h"

using namespace std;
using namespace SimpleBLE;

OscManager fOscManager;

int main() {
    fOscManager.send(12.3);

    auto adapter_optional = Utils::getAdapter();

    if (!adapter_optional.has_value()) {
        return EXIT_FAILURE;
    }

    Adapter adapter = adapter_optional.value();

    std::vector<SimpleBLE::Peripheral> peripherals;

    adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral peripheral) { peripherals.push_back(peripheral); });

    adapter.set_callback_on_scan_start([]() { std::cout << "Scan started." << std::endl; });
    adapter.set_callback_on_scan_stop([]() { std::cout << "Scan stopped." << std::endl; });
    // Scan for 5 seconds and return.
    adapter.scan_for(5000);

    std::cout << "The following devices were found:" << std::endl;
    for (size_t i = 0; i < peripherals.size(); i++) {
        std::cout << "[" << i << "] " << peripherals[i].identifier() << " [" << peripherals[i].address() << "]"
                  << std::endl;
    }

    auto selection = Utils::getUserInputInt("Please select a device to connect to", peripherals.size() - 1);
    if (!selection.has_value()) {
        return EXIT_FAILURE;
    }

    auto peripheral = peripherals[selection.value()];
    std::cout << "Connecting to " << peripheral.identifier() << " [" << peripheral.address() << "]" << std::endl;
    peripheral.connect();

    std::cout << "Successfully connected." << std::endl;
    std::cout << "MTU: " << peripheral.mtu() << std::endl;
    for (auto &service: peripheral.services()) {
        std::cout << "Service: " << service.uuid() << std::endl;

        for (auto &characteristic: service.characteristics()) {
            std::cout << "  Characteristic: " << characteristic.uuid() << std::endl;

            std::cout << "    Capabilities: ";
            for (auto &capability: characteristic.capabilities()) {
                std::cout << capability << " ";
            }
            std::cout << std::endl;

            for (auto &descriptor: characteristic.descriptors()) {
                std::cout << "    Descriptor: " << descriptor.uuid() << std::endl;
            }
        }
    }

    std::cout << "+++ type 'exit', 'scan' or '?' " << std::endl;
    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "exit") {
            break;
        } else if (input == "scan") {
            cout << "re-scan " << endl;
        }
        std::cout << "Received input: " << input << std::endl;
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    peripheral.disconnect();
    fOscManager.finalize();
    return EXIT_SUCCESS;
}
