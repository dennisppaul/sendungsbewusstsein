#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include "cxxopts.hpp"

#include "simpleble/SimpleBLE.h"

#include "DeviceWHOOP4.h"
#include "utils.hpp"
#include "StringUtils.h"
#include "OscSenderReceiver.h"
#include "HeartBeat.h"

using namespace std;
using namespace SimpleBLE;

static const int NO_DEVICE_FOUND                      = -1;
static const int DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS = 5000;
static const int DEFAULT_SCAN_FOR_DEVICE_DURATION_MS  = 5000;

void scan_for_peripherals(Adapter &adapter,
                          vector<SimpleBLE::Peripheral> &peripherals,
                          int timeout_ms,
                          bool verbose) {
    peripherals.clear();

    adapter.set_callback_on_scan_found(
            [&](const SimpleBLE::Peripheral &peripheral) { peripherals.push_back(peripheral); });
    adapter.set_callback_on_scan_start([]() { cout << "scan started … "; });
    adapter.set_callback_on_scan_stop([]() { cout << "finished." << endl; });
    adapter.scan_for(timeout_ms);

    if (verbose) {
        cout << "The following devices were found:" << endl;
        for (size_t i = 0; i < peripherals.size(); i++) {
            cout << "[" << i << "] "
                 << peripherals[i].identifier()
                 << " [" << peripherals[i].address() << "]"
                 << endl;
        }
    }

//    for (size_t i = 0; i < peripherals.size(); i++) {
//        string connectable_string = peripherals[i].is_connectable() ? "Connectable" : "Non-Connectable";
//        string peripheral_string = peripherals[i].identifier() + " [" + peripherals[i].address() + "] " +
//                                        to_string(peripherals[i].rssi()) + " dBm";
//
//        cout << "[" << i << "] " << peripheral_string << " " << connectable_string << endl;
//
//        cout << "    Tx Power: " << dec << peripherals[i].tx_power() << " dBm" << endl;
//        cout << "    Address Type: " << peripherals[i].address_type() << endl;
//
//        vector<SimpleBLE::Service> services = peripherals[i].services();
//        for (auto& service : services) {
//            cout << "    Service UUID: " << service.uuid() << endl;
//            cout << "    Service data: ";
//            Utils::print_byte_array(service.data());
//        }
//
//        map<uint16_t, SimpleBLE::ByteArray> manufacturer_data = peripherals[i].manufacturer_data();
//        for (auto& [manufacturer_id, data] : manufacturer_data) {
//            cout << "    Manufacturer ID: " << manufacturer_id << endl;
//            cout << "    Manufacturer data: ";
//            Utils::print_byte_array(data);
//        }
//    }
}

int find_device(vector<SimpleBLE::Peripheral> &peripherals, string &device) {
    for (int i = 0; i < peripherals.size(); i++) {
        if (peripherals[i].identifier() == device) {
            cout << "+++ FOUND "
                 << peripherals[i].identifier()
                 << " [" << peripherals[i].address() << "]"
                 << " [" << i << "] "
                 << endl;
            return i;
        }
    }
    return NO_DEVICE_FOUND;
}

bool handle_connect(string &input) {
    cout << "connect_name @todo" << endl;
    vector<string>    tokens = split(input, ' ');
    for (const string &t: tokens) {
        cout << t << endl;
    }
    return false;
}

void print_help() {
    static const string INDENTION = "  ";
    cout << "Sendungsbewusstsein, broadcast BLE devices via OSC." << endl;
    cout << endl;
    cout << "Available Commands:" << endl;
    cout << INDENTION << "scan      " << "for available devices" << endl;
    cout << INDENTION << "connect   " << "to device either by name ( e.g 'connect name WHOOP' )" << endl;
    cout << INDENTION << "          " << "or by address ( e.g 'connect address 225C6FBA-C7E8-0494-34C6-A54DF25AF596' )"
         << endl;
    cout << INDENTION << "exit      " << "leave application" << endl;
    cout << INDENTION << "help      " << "display this message" << endl;
    cout << endl;
}

bool process_input(Adapter &adapter, vector<SimpleBLE::Peripheral> &peripherals, string &input) {
    if (input.starts_with("exit")) {
        cout << "exiting application" << endl;
        return true;
    }

    if (input.starts_with("scan")) {
        scan_for_peripherals(adapter, peripherals, DEFAULT_SCAN_FOR_DEVICE_DURATION_MS, true);
        return false;
    }

    if (input.starts_with("connect")) {
        return handle_connect(input);
    }

    if (input == "help") {
        print_help();
        return false;
    }

    cout << "+++ command not recognized: " << input << endl << endl;
    print_help();
    return false;
}

void init_heart_beat(HeartBeat &heart_beat) {
    heart_beat.start();
    heart_beat.set_frequency(DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS);
}

void print_prompt() {
    cout << "> ";
    cout.flush();
}

bool parse_input(int argc, char *argv[]) {
    try {
        cxxopts::Options options("sendungsbewusstsein", "sendungsbewusstsein broadcasts BLE devices via OSC.");
        options
                .positional_help("[optional args]")
                .show_positional_help()
                .set_tab_expansion()
//                .allow_unrecognised_options()
                .add_options()
                        ("h,help", "print this help message")
                        ("q,quit", "quit application")
                        ("s,scan", "scan for available devices", cxxopts::value<string>())
                        ("w,watchdog",
                         "frequency of watchdog singals in milliseconds (ms) ( default: 5000ms ). a value of 0 turns the watchdog off.",
                         cxxopts::value<int>())
                        ("c,connect",
                         "connect to device either by name ( e.g '-c name WHOOP' ), by address ( e.g '-c address C6FBA-C7E8-0494-34C6-A54DF25AF596' ) or by index ( e.g '-c index 3' ) ( note that the indices might change after a re-scanning for devices )",
                         cxxopts::value<vector<string>>())
                        ("d,disconnect", "disconnect from device either by name, address or index",
                         cxxopts::value<vector<string>>());
//        cout << options.help({"", "Group"}) << endl;
        options.parse_positional({"connect", "disconnect"});

        auto result = options.parse(argc, argv);
//        cout << result.arguments_string() << endl;

        if (result.count("help")) {
            cout << options.help() << endl;
            return false;
        }

        if (result.count("quit")) {
            cout << "quit" << endl;
            return true;
        }

        if (result.count("input")) {
            string inputFile = result["input"].as<string>();
            cout << "Input file: " << inputFile << endl;
        }

        if (result.count("watchdog")) {
            cout << "watchdog frequency = " << result["watchdog"].as<int>() << endl;
        }

        if (result.count("output")) {
            string outputFile = result["output"].as<string>();
            cout << "Output file: " << outputFile << endl;
        }

        if (result.count("connect")) {
            cout << "Positional = {";
            auto            &v = result["connect"].as<vector<string>>();
            for (const auto &s: v) {
                cout << s << ", ";
            }
            cout << "}" << endl;
        }
    }
    catch (const cxxopts::exceptions::exception &e) {
        cout << "error parsing options: " << e.what() << endl;
        return true;
    }
    return false;
}

bool parse_input(vector<string> &args_vec) {
    int         argc   = static_cast<int>(args_vec.size());
    char        **argv = new char *[args_vec.size()];
    for (size_t i      = 0; i < args_vec.size(); ++i) {
        argv[i] = new char[args_vec[i].length() + 1];
        strcpy(argv[i], args_vec[i].c_str());
    }
    bool        mExit  = parse_input(argc, argv);
    delete[] argv;
    return mExit;
}

void connect_to_devices_by_name(OscSenderReceiver &osc_manager,
                                vector<SimpleBLE::Peripheral> &peripherals,
                                vector<SimpleBLE::Peripheral> &connected_peripherals,
                                vector<string> &device_names) {
    cout << "+++ CONNECT TO DEVICES" << endl;
    const static int NUM_OF_DEVICES = static_cast<int>(device_names.size());

    int mPeripheralID[NUM_OF_DEVICES];

    for (size_t i = 0; i < NUM_OF_DEVICES; i++) {
        mPeripheralID[i] = find_device(peripherals, device_names[i]);
        if (mPeripheralID[i] != NO_DEVICE_FOUND) {
            auto peripheral = peripherals[mPeripheralID[i]];
            cout << "Connecting to " << peripheral.identifier() << " [" << peripheral.address() << "]"
                 << endl;
            connected_peripherals.push_back(peripheral);

//            // store all service and characteristic uuids in a vector as 'service + characteristic' pair
//            vector<pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuids;
//
//            for (auto service: peripheral.services()) {
//                for (auto characteristic: service.characteristics()) {
//                    uuids.emplace_back(service.uuid(), characteristic.uuid());
//                }
//            }
//            cout << "The following services and characteristics were found:" << endl;
//            for (size_t j = 0; j < uuids.size(); j++) {
//                cout << "[" << j << "] " << uuids[j].first << " " << uuids[j].second << endl;
//            }
//            // Subscribe to the characteristic.
//            int         selection = 0; /* ---------------------- */
//            peripheral.notify(uuids[selection].first, uuids[selection].second, [&](SimpleBLE::ByteArray bytes) {
//                cout << "Received: ";
//                Utils::print_byte_array(bytes);
//            });
//            peripheral.unsubscribe(uuids[selection.value()].first, uuids[selection.value()].second);
        }
    }

    /* iterate over connected peripherals and instantiate according classes */
    for (size_t i = 0; i < connected_peripherals.size(); i++) {
        Peripheral peripheral = connected_peripherals[i];
        if (peripheral.identifier() == string(DeviceWHOOP4::NAME)) {
            DeviceWHOOP4(i, peripheral, &osc_manager);
        }
    }
}

void print_device_capabilities(vector<SimpleBLE::Peripheral> &peripherals) {
    for (auto peripheral: peripherals) {
        try {
            string connectable_string = peripheral.is_connectable() ? "connectable" : "non-connectable";
            string peripheral_string  = peripheral.identifier() + " [" + peripheral.address() + "] " +
                                        to_string(peripheral.rssi()) + " dBm";
            cout << peripheral_string << " " << connectable_string << endl;
//            cout << "MTU: " << peripheral.mtu() << endl; // Maximum Transmission Unit.
            if (peripheral.is_connectable()) {
                const bool mIsConnected = peripheral.is_connected();
                if (!mIsConnected) {
                    cout << "connect";
                    peripheral.connect();
                    cout << "ed … successfully" << endl;
                } else {
                    cout << "already connected." << endl;
                    break;
                }

                for (auto &service: peripheral.services()) {
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
                if (!mIsConnected) {
                    cout << "disconnect";
                    peripheral.disconnect();
                    cout << "ed … successfully" << endl;
                }
            } else {
                cout << "cannot connect" << endl;
            }
        } catch (const exception &e) {
            cout << "error: " << e.what() << endl;
        }
    }
}

int main(int argc, char *argv[]) {
    OscSenderReceiver fOscManager;
    HeartBeat         fHeartBeat(&fOscManager);
    init_heart_beat(fHeartBeat);

    /* connect to adapter */
    auto adapter_optional = Utils::getAdapter();
    if (!adapter_optional.has_value()) {
        cerr << "+++ could not find BLE adapter" << endl;
        return EXIT_FAILURE;
    }
    Adapter                       adapter = adapter_optional.value();
    vector<SimpleBLE::Peripheral> peripherals;
    vector<SimpleBLE::Peripheral> connected_peripherals;
    bool                          mExit   = false;

    // TODO should the application always scan for device in the beginning or only if there are CLI args?
    // TODO rename heartbeat to watchdog in code
    // TODO add option to turn off watchdog

    /* perform one intial scan before parsing CLI args if there are CLI args */
//    if (argc > 1) {
    cout << "+++ SCAN FOR AVAILABLE DEVICES" << endl;
    scan_for_peripherals(adapter, peripherals, DEFAULT_SCAN_FOR_DEVICE_DURATION_MS, true);
    /* for testing purposes */
    vector<string> device_names = {DeviceWHOOP4::NAME};
    connect_to_devices_by_name(fOscManager, peripherals, connected_peripherals, device_names);
//    }

//    scan_for_peripherals(adapter, peripherals, DEFAULT_SCAN_FOR_DEVICE_DURATION_MS);
//    print_device_capabilities(peripherals);
//    mExit = parse_input(argc, argv); /* parse CLI args */

    /* handle CLI input */
    string input;
    print_prompt();
    while (!mExit && getline(cin, input)) {
        if (!input.empty()) {
//            vector<string> args_vec = {"sendungsbewusstsein", "-h"};
//            parse_input(args_vec);

            mExit = process_input(adapter, peripherals, input);
        }
        print_prompt();
    }

    for (Peripheral peripheral: connected_peripherals) {
        cout << "disconnected … " << endl;
        peripheral.disconnect();
        cout << "successfully" << endl;
    }

    cout << "++ shutting down OSC … " << endl;
    fOscManager.finalize();
    cout << "successfully" << endl;
    return EXIT_SUCCESS;
}

//    vector<string> args_vec = {"sendungsbewusstsein",
//                               "-h",
//                               "--float", "100",
//                               "pos", "itional",
//                               "-i", "somefile",
//                               "--connect", "one", "two", "three"};
//    parse_input(args_vec);

//    string                 _input = "Hello World How Are, You, Today";
//    vector<string>    tokens = split(_input, ' ');
//    for (const string &t: tokens) {
//        cout << t << endl;
//    }
//
//    string                 delimiter = ", ";
//    vector<string>    _tokens   = split(_input, delimiter);
//    for (const string &t: _tokens) {
//        cout << t << endl;
//    }
