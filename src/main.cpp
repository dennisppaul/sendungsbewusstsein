#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include "cxxopts.hpp"

#include "simpleble/SimpleBLE.h"

#include "Device.h"
#include "DeviceWHOOP4.h"
#include "DeviceWahooKICKR.h"
#include "utils.hpp"
#include "StringUtils.h"
#include "OscSenderReceiver.h"
#include "HeartBeat.h"

using namespace std;
using namespace SimpleBLE;

static const int    NO_DEVICE_FOUND                      = -1;
static const int    DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS = 2000;
static const int    DEFAULT_SCAN_FOR_DEVICE_DURATION_MS  = 5000;
static const string PROMPT_TOKE                          = "> ";
static const string CONNECTION_TYPE_NAME                 = "name";
static const string CONNECTION_TYPE_ADDRESS              = "address";
static const string CONNECTION_TYPE_INDEX                = "index";

vector<string> registered_device_names = {DeviceWHOOP4::NAME};
vector<Device> connected_devices;
int            fCurrentDeviceID        = NO_DEVICE_FOUND;

void scan_for_peripherals(Adapter &adapter,
                          vector<SimpleBLE::Peripheral> &peripherals,
                          int timeout_ms,
                          bool verbose) {
    cout << "+++ scanning for available devices ( " << timeout_ms << "ms )" << endl;

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

int find_device_by_name(vector<SimpleBLE::Peripheral> &peripherals, const string &name) {
    for (int i = 0; i < peripherals.size(); i++) {
        if (peripherals[i].identifier() == name) {
            cout << "+++ FOUND by name > "
                 << " name:[" << peripherals[i].identifier() << "]"
                 << " address:[" << peripherals[i].address() << "]"
                 << " index:[" << i << "] "
                 << endl;
            return i;
        }
    }
    return NO_DEVICE_FOUND;
}

int find_device_by_address(vector<SimpleBLE::Peripheral> &peripherals, const string &address) {
    for (int i = 0; i < peripherals.size(); i++) {
        if (peripherals[i].address() == address) {
            cout << "+++ FOUND by address > "
                 << " name:[" << peripherals[i].identifier() << "]"
                 << " address:[" << peripherals[i].address() << "]"
                 << " index:[" << i << "] "
                 << endl;
            return i;
        }
    }
    return NO_DEVICE_FOUND;
}

void failed_connection_type(const string &type) {
    cout << "+++ only the following connection types are allowed:" << endl;
    cout << CONNECTION_TYPE_NAME << endl;
    cout << CONNECTION_TYPE_ADDRESS << endl;
    cout << CONNECTION_TYPE_INDEX << endl;
    cout << "+++ found connection type: " << endl;
    cout << type << endl;
}

bool compile_device_IDs(vector<int> &device_IDs,
                        vector<SimpleBLE::Peripheral> &peripherals,
                        const string &type,
                        const vector<string> &input) {
    if (type.starts_with(CONNECTION_TYPE_NAME)) {
        cout << "@todo connect " << CONNECTION_TYPE_NAME << endl;
        for (const string &t: input) {
            int mPeripheralID = find_device_by_name(peripherals, t);
            if (mPeripheralID != NO_DEVICE_FOUND) {
                device_IDs.push_back(mPeripheralID);
            } else {
                cout << "could not find name '" << t << "'." << endl;
            }
        }
        return !device_IDs.empty();
    } else if (type.starts_with(CONNECTION_TYPE_ADDRESS)) {
        cout << "@todo connect " << CONNECTION_TYPE_ADDRESS << endl;
        for (const string &t: input) {
            int mPeripheralID = find_device_by_address(peripherals, t);
            if (mPeripheralID != NO_DEVICE_FOUND) {
                device_IDs.push_back(mPeripheralID);
            } else {
                cout << "could not find address '" << t << "'." << endl;
            }
        }
        return !device_IDs.empty();
    } else if (type.starts_with(CONNECTION_TYPE_INDEX)) {
        cout << "@todo connect " << CONNECTION_TYPE_INDEX << endl;
        for (const string &t: input) {
            int mPeripheralID = NO_DEVICE_FOUND;
            try {
                mPeripheralID = stoi(t);
            } catch (const exception &e) {
                cout << "could not convert index '" << t << "' to number: " << e.what() << endl;
            }
            if (mPeripheralID > NO_DEVICE_FOUND && mPeripheralID < peripherals.size()) {
                device_IDs.push_back(mPeripheralID);
            } else {
                cout << "could not find index '" << t << "'." << endl;
            }
        }
        return !device_IDs.empty();
    }

    failed_connection_type(type);
    return false;
}

vector<string> sanitze_device_identifiers(const vector<string> &input) {
    vector<string> mInput = vector<string>(input);
    for (string    &str: mInput) {
        str.erase(remove(str.begin(), str.end(), '\"'), str.end());
    }
    return mInput;
}

int handle_connect(vector<SimpleBLE::Peripheral> &peripherals,
                   const string &type,
                   const vector<string> &input,
                   int current_device_ID) {
    vector<int> mDeviceIDs;
    bool        mSuccess = compile_device_IDs(mDeviceIDs,
                                              peripherals,
                                              type,
                                              sanitze_device_identifiers(input));

    if (!mSuccess) {
        cout << "+++ could not connect to any device." << endl;
        return current_device_ID;
    }

    cout << "+++ found " << mDeviceIDs.size() << " to connect to." << endl;
    for (int i: mDeviceIDs) {
        auto peripheral = peripherals[i];
        // TODO actually connect to devices
        if (peripheral.identifier() == string(DeviceWHOOP4::NAME)) {
            cout << peripheral.identifier() << " <> " << string(DeviceWHOOP4::NAME) << endl;
            current_device_ID++;
            DeviceWHOOP4(current_device_ID, peripheral);
//            connected_peripherals.push_back(peripheral); // TODO this need to be handle VERY differently
        } else if (peripheral.identifier() == string(DeviceWahooKICKR::NAME)) {
            cout << peripheral.identifier() << " <> " << string(DeviceWahooKICKR::NAME) << endl;
        }
    }
    return current_device_ID;
}

void handle_disconnect(const string &type, const vector<string> &input) {
    cout << "disconnect @todo" << endl;
    cout << "type: " << type << endl;

    for (const string &t: input) {
        cout << t << endl;
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

void init_heart_beat(HeartBeat &heart_beat) {
    heart_beat.start();
    heart_beat.set_frequency(DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS);
}

void print_prompt() {
    cout << PROMPT_TOKE;
    cout.flush();
}

bool parse_input(Adapter &adapter,
                 vector<SimpleBLE::Peripheral> &peripherals,
                 int argc, char *argv[]) {
    static const string CMD_CONNECT          = "connect";
    static const string CMD_CONNECT_CMD      = "" + CMD_CONNECT;
    static const string CMD_DISCONNECT       = "disconnect";
    static const string CMD_DISCONNECT_CMD   = "" + CMD_DISCONNECT;
    static const string CMD_PERIPHERALS      = "peripherals";
    static const string CMD_PERIPHERALS_CMD  = "p," + CMD_PERIPHERALS;
    static const string DEFAULT_CONNECT_TYPE = CONNECTION_TYPE_NAME;
    try {
        cxxopts::Options commands("sendungsbewusstsein", "sendungsbewusstsein broadcasts BLE devices via OSC.");
        commands
                .set_tab_expansion()
                .set_width(100)
                .positional_help("[optional args]")
                .show_positional_help()
//                .allow_unrecognised_options()
                .add_options()
                        ("h,help", "print this help message")
                        ("q,quit", "quit application")
                        ("i,info", "print capabilities of all available devices")
                        ("s,scan", "scan for available devices",
                         cxxopts::value<int>()->implicit_value(to_string(DEFAULT_SCAN_FOR_DEVICE_DURATION_MS)))
                        ("w,watchdog",
                         "frequency of watchdog singals in milliseconds. a value of 0 turns the watchdog off.",
                         cxxopts::value<int>()->implicit_value(to_string(DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS)))
                        (CMD_PERIPHERALS_CMD,
                         "specify peripherals for connect or disconnect. names that contain space need to be surrounded by quotation marks. indices might change after scan.",
                         cxxopts::value<vector<string>>())
                        (CMD_CONNECT_CMD,
                         "connect to device either by name (e.g '-p \"WHOOP 4A0934182\" --connect=name'), by address (e.g '-p C6FBA-C7E8-0494-34C6-A54DF25AF596 --connect=address') or by index (e.g '-p 3 --connect=index').",
                         cxxopts::value<string>()->implicit_value(DEFAULT_CONNECT_TYPE))
                        (CMD_DISCONNECT_CMD,
                         "disconnect from device either by name, address or index (see 'connect')",
                         cxxopts::value<string>()->implicit_value(DEFAULT_CONNECT_TYPE));
        commands.parse_positional({CMD_PERIPHERALS, ""});
        auto result = commands.parse(argc, argv);

        if (result.count("help")) {
            cout << commands.help() << endl;
            return false;
        }

        if (result.count("info")) {
            cout << "info" << endl;
            print_device_capabilities(peripherals);
            return false;
        }

        if (result.count("quit")) {
            cout << "quit" << endl;
            return true;
        }

        if (result.count("scan")) {
            int mScanDuartion = result["scan"].as<int>(); // default: DEFAULT_SCAN_FOR_DEVICE_DURATION_MS
            scan_for_peripherals(adapter, peripherals, mScanDuartion, true);
        }

        if (result.count("watchdog")) {
            cout << "watchdog frequency = " << result["watchdog"].as<int>() << endl;
            return false;
        }

        vector<string> mPeripherals;
        if (result.count(CMD_PERIPHERALS)) {
            mPeripherals = result[CMD_PERIPHERALS].as<vector<string >>();
            cout << "peripherals (" << mPeripherals.size() << ") = {";
            for (const auto &s: mPeripherals) {
                cout << s << ", ";
            }
            cout << "}" << endl;
        }

        if ((result.count(CMD_CONNECT) || result.count(CMD_DISCONNECT)) && mPeripherals.empty()) {
            cout << "no peripheral(s) specified to connect to or disconnect from." << endl;
        } else {
            if (result.count(CMD_CONNECT)) {
                string mType = result[CMD_CONNECT].as<string>();
                fCurrentDeviceID = handle_connect(peripherals, mType, mPeripherals, fCurrentDeviceID);
            } else if (result.count(CMD_DISCONNECT)) {
                string mType = result[CMD_DISCONNECT].as<string>();
                cout << "type: " << mType << endl;
                handle_disconnect(mType, mPeripherals);
            }
        }

        if (!result.unmatched().empty()) {
            cout << "unmatched commands: ";
            for (const auto &command: result.unmatched()) {
                cout << "'" << command << "' ";
            }
            cout << endl;
        }
    }
    catch (const cxxopts::exceptions::exception &e) {
        cout << "ERROR: " << e.what() << endl;
    }
    return false;
}

bool parse_input_vec(Adapter &adapter,
                     vector<SimpleBLE::Peripheral> &peripherals,
                     vector<string> &args_vec) {
    int         argc   = static_cast<int>(args_vec.size());
    char        **argv = new char *[args_vec.size()];
    for (size_t i      = 0; i < args_vec.size(); ++i) {
        argv[i] = new char[args_vec[i].length() + 1];
        strcpy(argv[i], args_vec[i].c_str());
    }

    bool mExit = parse_input(adapter, peripherals, argc, argv);
    delete[] argv;
    return mExit;
}

//void connect_to_devices_by_name(OscSenderReceiver &osc_manager,
//                                vector<SimpleBLE::Peripheral> &peripherals,
//                                vector<SimpleBLE::Peripheral> &connected_peripherals,
//                                vector<string> &device_names) {
//    cout << "+++ CONNECT TO DEVICES" << endl;
//    const static int NUM_OF_DEVICES = static_cast<int>(device_names.size());
//
//    int mPeripheralID[NUM_OF_DEVICES];
//
//    for (size_t i = 0; i < NUM_OF_DEVICES; i++) {
//        mPeripheralID[i] = find_device(peripherals, device_names[i]);
//        if (mPeripheralID[i] != NO_DEVICE_FOUND) {
//            auto peripheral = peripherals[mPeripheralID[i]];
//            cout << "connecting to " << peripheral.identifier() << " [" << peripheral.address() << "]" << endl;
//            if (peripheral.identifier() == string(DeviceWHOOP4::NAME)) {
//                DeviceWHOOP4(i, peripheral, &osc_manager);
//            }
//            connected_peripherals.push_back(peripheral);
//
////            // store all service and characteristic uuids in a vector as 'service + characteristic' pair
////            vector<pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> uuids;
////
////            for (auto service: peripheral.services()) {
////                for (auto characteristic: service.characteristics()) {
////                    uuids.emplace_back(service.uuid(), characteristic.uuid());
////                }
////            }
////            cout << "The following services and characteristics were found:" << endl;
////            for (size_t j = 0; j < uuids.size(); j++) {
////                cout << "[" << j << "] " << uuids[j].first << " " << uuids[j].second << endl;
////            }
////            // Subscribe to the characteristic.
////            int         selection = 0; /* ---------------------- */
////            peripheral.notify(uuids[selection].first, uuids[selection].second, [&](SimpleBLE::ByteArray bytes) {
////                cout << "Received: ";
////                Utils::print_byte_array(bytes);
////            });
////            peripheral.unsubscribe(uuids[selection.value()].first, uuids[selection.value()].second);
//        }
//    }
//}

int main(int argc, char *argv[]) {
    HeartBeat fHeartBeat;
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

    // TODO rename heartbeat to watchdog in code
    // TODO add option to turn off watchdog

    /* perform one intial scan before parsing CLI args if there are CLI args */
    if (argc > 1) {
        mExit = parse_input(adapter, peripherals, argc, argv); /* parse CLI args */
    }

    /* handle CLI input */
    string input;
    print_prompt();
    while (!mExit && getline(cin, input)) {
        if (!input.empty()) {

            vector<string> commands = split(input, " ");
            commands.insert(commands.begin(), PROMPT_TOKE);

#ifdef DEBUG_COMMAND
            cout << "CMD: " << commands.size() << endl;
            for (const auto &s: commands) {
                cout << s << ", ";
            }
            cout << endl;
#endif

            mExit = parse_input_vec(adapter, peripherals, commands);
        }
        print_prompt();
    }

    for (Peripheral peripheral: connected_peripherals) {
        cout << "disconnected … " << endl;
        peripheral.disconnect();
        cout << "successfully" << endl;
    }

    cout << "+++ shutting down OSC … " << endl;
    OscSenderReceiver::instance().finalize();
    cout << "successfully" << endl;
    return EXIT_SUCCESS;
}

