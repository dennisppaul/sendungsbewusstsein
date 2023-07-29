// TODO implement saving connected devices ( + disconnect fucntionality )
// TODO implement writing to BLE devices
// TODO maybe change architecture to a model where characteristics can be subscribed to e.g ( subscribe to `SERVICE_CYCLING_POWER + CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N` or `SERVICE_FITNESS_MACHINE + CHARACTERISTIC_INDOOR_BIKE_DATA_N` ) this would also require to querry a device for available services/characteristic pairs

#include <iostream>
#include <vector>
#include <chrono>
#include <string>

#include "cxxopts.hpp"
#include "simpleble/SimpleBLE.h"

#include "Console.h"
#include "Device.h"
#include "utils.hpp"
#include "StringUtils.h"
#include "OscSenderReceiver.h"
#include "Watchdog.h"

#include "CharacteristicsGATT.h"
#include "CharacteristicHeartRateMeasurment.h"
#include "CharacteristicCyclingPowerMeasurement.h"

using namespace std;
using namespace SimpleBLE;

static const int    NO_DEVICE_FOUND                      = -1;
static const int    DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS = 2000;
static const int    DEFAULT_SCAN_FOR_DEVICE_DURATION_MS  = 5000;
static const string PROMPT_TOKE                          = "> ";
static const string CONNECTION_TYPE_NAME                 = "name";
static const string CONNECTION_TYPE_ADDRESS              = "address";
static const string CONNECTION_TYPE_INDEX                = "index";

vector<Device *> connected_devices;
int              fCurrentDeviceID   = NO_DEVICE_FOUND;
int              fWatchdogFrequency = DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS;

struct ApplicationProperties {
    string OSC_address           = DEFAULT_OSC_TRANSMIT_ADDRESS;
    int    OSC_transmit_port     = DEFAULT_OSC_TRANSMIT_PORT;
    int    OSC_receive_port      = DEFAULT_OSC_RECEIVE_PORT;
    bool   OSC_use_UDP_multicast = DEFAULT_USE_UDP_MULTICAST;

} default_application_properties;

void scan_for_peripherals(Adapter &adapter,
                          vector<SimpleBLE::Peripheral> &peripherals,
                          int timeout_ms,
                          bool verbose) {
    console << "scanning for available devices ( " << timeout_ms << "ms )" << endl;

    peripherals.clear();

    adapter.set_callback_on_scan_found(
            [&](const SimpleBLE::Peripheral &peripheral) { peripherals.push_back(peripheral); });
    adapter.set_callback_on_scan_start([]() { console << "scan started … "; });
    adapter.set_callback_on_scan_stop([]() { console << "finished." << endl; });
    adapter.scan_for(timeout_ms);

    if (verbose) {
        console << "the following devices were found:" << endl;
        for (size_t i = 0; i < peripherals.size(); i++) {
            console << "[" << i << "] "
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
//        console << "[" << i << "] " << peripheral_string << " " << connectable_string << endl;
//
//        console << "    Tx Power: " << dec << peripherals[i].tx_power() << " dBm" << endl;
//        console << "    Address Type: " << peripherals[i].address_type() << endl;
//
//        vector<SimpleBLE::Service> services = peripherals[i].services();
//        for (auto& service : services) {
//            console << "    Service UUID: " << service.uuid() << endl;
//            console << "    Service data: ";
//            Utils::print_byte_array(service.data());
//        }
//
//        map<uint16_t, SimpleBLE::ByteArray> manufacturer_data = peripherals[i].manufacturer_data();
//        for (auto& [manufacturer_id, data] : manufacturer_data) {
//            console << "    Manufacturer ID: " << manufacturer_id << endl;
//            console << "    Manufacturer data: ";
//            Utils::print_byte_array(data);
//        }
//    }
}

int find_device_by_name(vector<SimpleBLE::Peripheral> &peripherals, const string &name) {
    for (int i = 0; i < peripherals.size(); i++) {
        // TODO check if it is better to test for equality or `starts_with`
        if (starts_with_ignore_case(peripherals[i].identifier(), name)) {
//        if (peripherals[i].identifier().starts_with(name)) {
//        if (peripherals[i].identifier() == name) {
            console << "FOUND by name >"
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
            console << "FOUND by address >"
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
    console << "only the following connection types are allowed:" << endl;
    console << CONNECTION_TYPE_NAME << endl;
    console << CONNECTION_TYPE_ADDRESS << endl;
    console << CONNECTION_TYPE_INDEX << endl;
    console << "found connection type: " << endl;
    console << type << endl;
}

bool compile_device_IDs(vector<int> &device_IDs,
                        vector<SimpleBLE::Peripheral> &peripherals,
                        const string &type,
                        const vector<string> &input) {
    if (type.starts_with(CONNECTION_TYPE_NAME)) {
        console << "@todo connect '" << CONNECTION_TYPE_NAME << "'" << endl;
        for (const string &t: input) {
            int mPeripheralID = find_device_by_name(peripherals, t);
            if (mPeripheralID != NO_DEVICE_FOUND) {
                device_IDs.push_back(mPeripheralID);
            } else {
                console << "could not find name '" << t << "'." << endl;
            }
        }
        return !device_IDs.empty();
    } else if (type.starts_with(CONNECTION_TYPE_ADDRESS)) {
        console << "@todo connect '" << CONNECTION_TYPE_ADDRESS << "'" << endl;
        for (const string &t: input) {
            int mPeripheralID = find_device_by_address(peripherals, t);
            if (mPeripheralID != NO_DEVICE_FOUND) {
                device_IDs.push_back(mPeripheralID);
            } else {
                console << "could not find address '" << t << "'." << endl;
            }
        }
        return !device_IDs.empty();
    } else if (type.starts_with(CONNECTION_TYPE_INDEX)) {
        console << "@todo connect " << CONNECTION_TYPE_INDEX << endl;
        for (const string &t: input) {
            int mPeripheralID = NO_DEVICE_FOUND;
            try {
                mPeripheralID = stoi(t);
            } catch (const exception &e) {
                console << "could not convert index '" << t << "' to number: " << e.what() << endl;
            }
            if (mPeripheralID > NO_DEVICE_FOUND && mPeripheralID < peripherals.size()) {
                device_IDs.push_back(mPeripheralID);
            } else {
                console << "could not find index '" << t << "'." << endl;
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
        console << "could not connect to any device." << endl;
        return current_device_ID;
    }

    console << "found " << mDeviceIDs.size() << " device" << (mDeviceIDs.size() > 1 ? "s" : "") << " to connect to."
            << endl;
    for (int i: mDeviceIDs) {
        auto peripheral = peripherals[i];

        /* test for supported characteristics */
        current_device_ID++;
        Device *mDevice = new Device(current_device_ID, &peripheral);

//        // TODO actually connect to devices
//        if (peripheral.identifier().starts_with("WHOOP")) {
//            current_device_ID++;
//            // TODO safe instance somehow
//            auto *device = new DeviceWHOOP4(current_device_ID, peripheral);
//            console << peripheral.identifier() << " <> " << string(device->name()) << endl;
//            connected_devices.push_back(device);
////            connected_peripherals.push_back(peripheral); // TODO this need to be handle VERY differently
//        } else if (peripheral.identifier().starts_with("Wahoo KICKR")) {
//            current_device_ID++;
//            // TODO safe instance somehow
//            auto *device = new DeviceWahooKICKR(current_device_ID, peripheral);
//            console << peripheral.identifier() << " <> " << string(device->name()) << endl;
//            connected_devices.push_back(device);
////            connected_peripherals.push_back(peripheral); // TODO this need to be handle VERY differently
//        } else if (peripheral.identifier().starts_with("KICKR CORE")) {
//            current_device_ID++;
//            // TODO safe instance somehow
//            // TODO this uses KICKR class for KICKR CORE
//            auto *device = new DeviceWahooKICKR(current_device_ID, peripheral);
//            console << peripheral.identifier() << " <> " << string(device->name())
//                    << " ( note this is just a very dirty hack to test if 'KICKR CORE' works like 'Wahoo KICKR' )"
//                    << endl;
//            connected_devices.push_back(device);
//        } else {
//            cerr << "+++ could not connect to device " << peripheral.identifier() << endl;
//            break;
//        }
        console << "connected OSC device ID of device "
                << peripheral.identifier()
                << " is "
                << current_device_ID
                << endl;
    }
    return current_device_ID;
}

void handle_disconnect(const string &type, const vector<string> &input) {
    console << "disconnect @todo" << endl;
    console << "type: " << type << endl;

    for (const string &t: input) {
        console << t << endl;
    }
//    `connected_devices`;

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
                            console << capability << " ";
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

void init_watchdog(Watchdog &watchdog) {
    watchdog.start();
    watchdog.set_frequency(fWatchdogFrequency);
}

void print_prompt() {
    cout << PROMPT_TOKE;
    cout.flush();
}

bool parse_input(Adapter &adapter,
                 vector<SimpleBLE::Peripheral> &peripherals,
                 Watchdog *watchdog,
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
                        (CMD_PERIPHERALS_CMD,
                         "specify peripherals for connect or disconnect. names that contain space need to be surrounded by quotation marks. indices might change after scan.",
                         cxxopts::value<vector<string>>())
                        (CMD_CONNECT_CMD,
                         "connect to device either by name (e.g '-p \"WHOOP 4A0934182\" --connect=name'), by address (e.g '-p C6FBA-C7E8-0494-34C6-A54DF25AF596 --connect=address') or by index (e.g '-p 3 --connect=index').",
                         cxxopts::value<string>()->implicit_value(DEFAULT_CONNECT_TYPE))
                        (CMD_DISCONNECT_CMD,
                         "disconnect from device either by name, address or index (see 'connect')",
                         cxxopts::value<string>()->implicit_value(DEFAULT_CONNECT_TYPE));

        commands.add_options("OSC")
                ("w,watchdog",
                 "frequency of watchdog singals in milliseconds. a value of 0 turns the watchdog off.",
                 cxxopts::value<int>()->implicit_value(to_string(DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS)))
                ("a,address", "OSC message transmit address",
                 cxxopts::value<std::string>()->implicit_value("127.0.0.1"))
                ("t,transmit", "port for transmitting messages", cxxopts::value<int>()->implicit_value("7000"))
                ("r,receive", "port for receiveing messages", cxxopts::value<int>()->implicit_value("7001"))
                ("m,multicast", "enable multicast broadcast", cxxopts::value<bool>()->implicit_value("false"));
        commands.parse_positional({CMD_PERIPHERALS, ""});
        auto result = commands.parse(argc, argv);

        if (result.count("help")) {
            console << commands.help() << endl;
            return false;
        }

        if (result.count("info")) {
            console << "info" << endl;
            print_device_capabilities(peripherals);
            return false;
        }

        if (result.count("quit")) {
            console << "quit" << endl;
            return true;
        }

        if (result.count("scan")) {
            int mScanDuartion = result["scan"].as<int>(); // default: DEFAULT_SCAN_FOR_DEVICE_DURATION_MS
            scan_for_peripherals(adapter, peripherals, mScanDuartion, true);
        }

        if (result.count("watchdog")) {
            console << "watchdog frequency = " << result["watchdog"].as<int>() << endl;
            fWatchdogFrequency = result["watchdog"].as<int>();
            if (watchdog != nullptr) {
                watchdog->set_frequency(fWatchdogFrequency);
            }
        }

        if (result.count("address")) {
            default_application_properties.OSC_address = result["address"].as<string>();
            console << "OSC address = " << default_application_properties.OSC_address << endl;
        }

        if (result.count("transmit")) {
            default_application_properties.OSC_transmit_port = result["transmit"].as<int>();
            console << "OSC transmit port = " << default_application_properties.OSC_transmit_port << endl;
        }

        if (result.count("receive")) {
            default_application_properties.OSC_receive_port = result["receive"].as<int>();
            console << "OSC receive port = " << default_application_properties.OSC_receive_port << endl;
        }

        if (result.count("multicast")) {
            default_application_properties.OSC_use_UDP_multicast = result["multicast"].as<bool>();
            console << "OSC use UDP multicast = " << default_application_properties.OSC_use_UDP_multicast << endl;
        }

        vector<string> mPeripherals;
        if (result.count(CMD_PERIPHERALS)) {
            mPeripherals = result[CMD_PERIPHERALS].as<vector<string >>();
            console << "peripherals (" << mPeripherals.size() << ") = {";
            for (const auto &s: mPeripherals) {
                console << s << ", ";
            }
            console << "}" << endl;
        }

        if ((result.count(CMD_CONNECT) || result.count(CMD_DISCONNECT)) && mPeripherals.empty()) {
            console << "no peripheral(s) specified to connect to or disconnect from." << endl;
        } else {
            if (result.count(CMD_CONNECT)) {
                string mType = result[CMD_CONNECT].as<string>();
                fCurrentDeviceID = handle_connect(peripherals, mType, mPeripherals, fCurrentDeviceID);
            } else if (result.count(CMD_DISCONNECT)) {
                string mType = result[CMD_DISCONNECT].as<string>();
                console << "type: " << mType << endl;
                handle_disconnect(mType, mPeripherals);
            }
        }

        if (!result.unmatched().empty()) {
            console << "unmatched commands: ";
            for (const auto &command: result.unmatched()) {
                console << "'" << command << "' ";
            }
            console << endl;
        }
    }
    catch (const cxxopts::exceptions::exception &e) {
        console << "ERROR: " << e.what() << endl;
    }
    return false;
}

bool parse_input_vec(Adapter &adapter,
                     vector<SimpleBLE::Peripheral> &peripherals,
                     Watchdog *watchdog,
                     vector<string> &args_vec) {
    int         argc   = static_cast<int>(args_vec.size());
    char        **argv = new char *[args_vec.size()];
    for (size_t i      = 0; i < args_vec.size(); ++i) {
        argv[i] = new char[args_vec[i].length() + 1];
        strcpy(argv[i], args_vec[i].c_str());
    }

    bool mExit = parse_input(adapter, peripherals, watchdog, argc, argv);
    delete[] argv;
    return mExit;
}

//void connect_to_devices_by_name(OscSenderReceiver &osc_manager,
//                                vector<SimpleBLE::Peripheral> &peripherals,
//                                vector<SimpleBLE::Peripheral> &connected_peripherals,
//                                vector<string> &device_names) {
//    console << "CONNECT TO DEVICES" << endl;
//    const static int NUM_OF_DEVICES = static_cast<int>(device_names.size());
//
//    int mPeripheralID[NUM_OF_DEVICES];
//
//    for (size_t i = 0; i < NUM_OF_DEVICES; i++) {
//        mPeripheralID[i] = find_device(peripherals, device_names[i]);
//        if (mPeripheralID[i] != NO_DEVICE_FOUND) {
//            auto peripheral = peripherals[mPeripheralID[i]];
//            console << "connecting to " << peripheral.identifier() << " [" << peripheral.address() << "]" << endl;
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
////            console << "The following services and characteristics were found:" << endl;
////            for (size_t j = 0; j < uuids.size(); j++) {
////                console << "[" << j << "] " << uuids[j].first << " " << uuids[j].second << endl;
////            }
////            // Subscribe to the characteristic.
////            int         selection = 0; /* ---------------------- */
////            peripheral.notify(uuids[selection].first, uuids[selection].second, [&](SimpleBLE::ByteArray bytes) {
////                console << "Received: ";
////                Utils::print_byte_array(bytes);
////            });
////            peripheral.unsubscribe(uuids[selection.value()].first, uuids[selection.value()].second);
//        }
//    }
//}

void osc_callback() {}

int main(int argc, char *argv[]) {
    console << "Sendungsbewusstsein" << endl;

    CharacteristicCyclingPowerMeasurement::register_characteristic();
    CharacteristicHeartRateMeasurment::register_characteristic();

//    unique_ptr<CharacteristicAbstract> mCyclingPowerMeasurement = CharacteristicFactory::create(SERVICE_CYCLING_POWER,
//                                                                                                CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N);
//    unique_ptr<CharacteristicAbstract> mHeartRateMeasurement    = CharacteristicFactory::create(SERVICE_HEART_RATE,
//                                                                                                CHARACTERISTIC_HEART_RATE_MEASUREMENT_N);
//    if (mHeartRateMeasurement) {
//        mHeartRateMeasurement->subscribe();
//    }

    /* connect to adapter */
    auto adapter_optional = Utils::getAdapter();
    if (!adapter_optional.has_value()) {
        cerr << "+++ could not find BLE adapter" << endl;
        return EXIT_FAILURE;
    }
    Adapter adapter = adapter_optional.value();
    console << "Using adapter: " << adapter.identifier() << " [" << adapter.address() << "]" << std::endl;
    vector<SimpleBLE::Peripheral> peripherals;
    vector<SimpleBLE::Peripheral> connected_peripherals;
    bool                          mExit = false;

    /* CLI args */
    if (argc > 1) {
        mExit = parse_input(adapter, peripherals, nullptr, argc, argv); /* parse CLI args */
    }

    /* OSC */
    OscSenderReceiver::init(default_application_properties.OSC_address.c_str(),
                            default_application_properties.OSC_transmit_port,
                            default_application_properties.OSC_receive_port,
                            default_application_properties.OSC_use_UDP_multicast);
    this_thread::sleep_for(std::chrono::seconds(2));

    /* watchdog */
    Watchdog watchdog; // TODO consider making `watchdog` global
    init_watchdog(watchdog);

    /* handle CLI input */
    // TODO replace this with console that is 'immune' to output i.e collect console output ( see linenoise )
    string input;
    print_prompt();
    while (!mExit && getline(cin, input)) {
        if (!input.empty()) {

            vector<string> commands = split(input, " ");
            commands.insert(commands.begin(), PROMPT_TOKE);

#ifdef DEBUG_COMMAND
            console << "CMD: " << commands.size() << endl;
            for (const auto &s: commands) {
                console << s << ", ";
            }
            console << endl;
#endif

            mExit = parse_input_vec(adapter, peripherals, &watchdog, commands);
        }
        print_prompt();
    }

    for (Peripheral peripheral: connected_peripherals) {
        console << "disconnected … " << endl;
        peripheral.disconnect();
        console << "successfully" << endl;
    }

    console << "shutting down OSC … " << endl;
    OscSenderReceiver::instance()->finalize();
    console << "successfully" << endl;
    return EXIT_SUCCESS;
}

