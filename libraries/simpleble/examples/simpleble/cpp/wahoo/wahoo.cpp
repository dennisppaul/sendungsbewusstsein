#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

#include "../common/utils.hpp"

#include "simpleble/SimpleBLE.h"

using namespace std::chrono_literals;
using namespace std;

/*
- Service: `00001818-0000-1000-8000-00805f9b34fb` > "cyclingPower"
    - Characteristic: `00002a63-0000-1000-8000-00805f9b34fb` > "cyclingPowerMeasurement" [notify]
*/

std::string DEVICE_NAME = "Wahoo KICKR";
std::string DEVICE_CHARACTERISTIC__CYCLING_POWER_MEASUREMENT = "00002a63-0000-1000-8000-00805f9b34fb";
static const int NO_DEVICE_FOUND = -1;

int getDeviceID(std::vector<SimpleBLE::Peripheral> &peripherals) {
	std::cout << "+++ searching for '" << DEVICE_NAME << "' … ";
    for (int i = 0; i < peripherals.size(); i++) {
	    std::string mDeviceIdentifier = peripherals[i].identifier();
    	if (mDeviceIdentifier.compare(0, DEVICE_NAME.length(), DEVICE_NAME) == 0) { 
			std::cout << "successful (" << i << ")" << std::endl;
    		return i;
		}
    }
	std::cout << "unsuccessful" << std::endl;
	return NO_DEVICE_FOUND;
}

int getCharacteristicID(SimpleBLE::Peripheral &peripheral) {
	int i = 0;
    for (auto service : peripheral.services()) {
        for (auto characteristic : service.characteristics()) {
 			std::string mCharacteristicIdentifier = characteristic.uuid();
	    	if (mCharacteristicIdentifier.compare(0, DEVICE_CHARACTERISTIC__CYCLING_POWER_MEASUREMENT.length(), DEVICE_CHARACTERISTIC__CYCLING_POWER_MEASUREMENT) == 0) { 
        		std::cout << "FOUND!!!" << std::endl;
	    		return i;
        	}
			i++;
        }
    }
	return NO_DEVICE_FOUND;
}

int main() {
	std::cout << "+++ Wahoo KICKR" << std::endl;

    auto adapter_optional = Utils::getAdapter();

    if (!adapter_optional.has_value()) {
        return EXIT_FAILURE;
    }

    auto adapter = adapter_optional.value();

    std::vector<SimpleBLE::Peripheral> peripherals;

    adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral peripheral) {
        std::cout << "Found device: " << peripheral.identifier() << " [" << peripheral.address() << "]" << std::endl;
        peripherals.push_back(peripheral);
    });

    adapter.set_callback_on_scan_start([]() { std::cout << "Scan started." << std::endl; });
    adapter.set_callback_on_scan_stop([]() { std::cout << "Scan stopped." << std::endl; });
    // Scan for 5 seconds and return.
    adapter.scan_for(5000);

    std::cout << "The following devices were found:" << std::endl;
    for (size_t i = 0; i < peripherals.size(); i++) {
        std::cout << "[" << i << "] " << peripherals[i].identifier() << " [" << peripherals[i].address() << "]" << std::endl;
    }

	int mDeviceID = getDeviceID(peripherals);
	if (mDeviceID == NO_DEVICE_FOUND) {
        return EXIT_FAILURE;
    }
    
    /* --- */

    auto peripheral = peripherals[mDeviceID];
    std::cout << "Connecting to " << peripheral.identifier() << " [" << peripheral.address() << "]" << std::endl;
    peripheral.connect();

    std::cout << "Successfully connected, printing services and characteristics.." << std::endl;

    // Store all service and characteristic uuids in a vector.
    std::vector<std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuids;
    for (auto service : peripheral.services()) {
        for (auto characteristic : service.characteristics()) {
// 			std::string mDeviceIdentifier = peripherals[i].identifier();
// 	    	if (characteristic.uuid().compare(0, DEVICE_CHARACTERISTIC__CYCLING_POWER_MEASUREMENT.length(), DEVICE_CHARACTERISTIC__CYCLING_POWER_MEASUREMENT) == 0) { 
//         	}
            uuids.push_back(std::make_pair(service.uuid(), characteristic.uuid()));
        }
    }

    std::cout << "The following services and characteristics were found:" << std::endl;
    for (size_t i = 0; i < uuids.size(); i++) {
        std::cout << "[" << i << "] " << uuids[i].first << " " << uuids[i].second << std::endl;
    }
    
    getCharacteristicID(peripheral);

    auto selection = Utils::getUserInputInt("Please select a characteristic to read", uuids.size() - 1);

    if (!selection.has_value()) {
        return EXIT_FAILURE;
    }

    // Subscribe to the characteristic.
    peripheral.notify(uuids[selection.value()].first, uuids[selection.value()].second, [&](SimpleBLE::ByteArray bytes) {
        std::cout << "Received: ";
        Utils::print_byte_array(bytes);
    });

    std::this_thread::sleep_for(60s);

    peripheral.unsubscribe(uuids[selection.value()].first, uuids[selection.value()].second);

    peripheral.disconnect();

    return EXIT_SUCCESS;
}
