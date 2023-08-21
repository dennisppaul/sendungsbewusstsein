#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <fstream>
#include <cstring>

#include "cxxopts.hpp"
#include "simpleble/SimpleBLE.h"

#include "main.h"
#include "Console.h"
#include "utils.hpp"
#include "StringUtils.h"
#include "Transceiver.h"
#include "Watchdog.h"
#include "TransceiverCommandHandler.h"

#include "CharacteristicHeartRateMeasurment.h"
#include "CharacteristicCyclingPowerMeasurement.h"
#include "CharacteristicCyclingPowerControlPoint.h"
#include "CharacteristicIndoorBikeData.h"
#include "CharacteristicFitnessMachineControlPoint.h"

// TODO add `reset_connected_devices` to CLI

using namespace std;
using namespace SimpleBLE;

static const int    DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS = 2000;
static const int    DEFAULT_SCAN_FOR_DEVICE_DURATION_MS  = 5000;
static const string PROMPT_TOKE                          = "> ";
static const string CONNECTION_TYPE_NAME                 = "name";
static const string CONNECTION_TYPE_ADDRESS              = "address";
static const string CONNECTION_TYPE_INDEX                = "index";

static int                                       fCurrentConnectedDeviceIndex = NO_DEVICE_FOUND;
static int                                       fWatchdogFrequency           = DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS;
static vector<shared_ptr<Device>>                fConnectedDevices;
static vector<shared_ptr<SimpleBLE::Peripheral>> fAvailablePeripherals;

static Adapter                   *fAdapter        = nullptr;
static TransceiverCommandHandler *fCommandHandler = nullptr;
static Watchdog                  *fWatchdog       = nullptr;
static shared_ptr<Device>        null_device      = nullptr;

struct ApplicationProperties {
    string OSC_address           = DEFAULT_OSC_TRANSMIT_ADDRESS;
    int    OSC_transmit_port     = DEFAULT_OSC_TRANSMIT_PORT;
    int    OSC_receive_port      = DEFAULT_OSC_RECEIVE_PORT;
    bool   OSC_use_UDP_multicast = DEFAULT_USE_UDP_MULTICAST;
}                                default_application_properties;

void scan_for_devices(int timeout_ms,
                      bool verbose,
                      bool ignore_devices_without_name) {
    if (fAdapter == nullptr) {
        error << "adapter is not initialized" << endl;
        return;
    }

    console
            << "scanning for available devices ( "
            << timeout_ms
            << "ms )"
            << endl;

    fAvailablePeripherals.clear();

    fAdapter->set_callback_on_scan_found([&](const SimpleBLE::Peripheral &peripheral) {
        console << ".";
        console.append();

        auto mPeripheral = make_shared<SimpleBLE::Peripheral>(peripheral);
        fAvailablePeripherals.push_back(mPeripheral);
    });

//    fAdapter->set_callback_on_scan_found([&](const SimpleBLE::Peripheral &mPeripheral) {
//        console << ".";
//        console.append();
//        fAvailablePeripherals.push_back(mPeripheral);
//    });
    fAdapter->set_callback_on_scan_start([]() {
        console << "scan started ";
        console.flush(false);
    });
    fAdapter->set_callback_on_scan_stop([]() {
        console << " finished.";
        console.append();
        console.newline();
    });
    fAdapter->scan_for(timeout_ms);

    if (verbose) {
        console
                << "the following devices were found"
                << (ignore_devices_without_name ? " ( ignoring devices without name )" : "")
                << ":"
                << endl;
        for (size_t i = 0; i < fAvailablePeripherals.size(); i++) {
            if (ignore_devices_without_name && fAvailablePeripherals[i]->identifier().empty()) continue;
            console << "[" << i << "] "
                    << fAvailablePeripherals[i]->identifier()
                    << " [" << fAvailablePeripherals[i]->address() << "]"
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

int get_number_of_available_devices() {
    return static_cast<int>(fAvailablePeripherals.size());
}

void reset_connected_devices() {
    /* note that this function just disconnects and removes all devices but does not inform about it */
    for (auto &mDevice: fConnectedDevices) {
        if (mDevice != nullptr) {
            if (mDevice->is_connected()) {
                mDevice->disconnect();
            }
        }
    }
    fCurrentConnectedDeviceIndex = NO_DEVICE_FOUND;
    fConnectedDevices.clear();
}

int get_number_of_connected_devices() {
    return static_cast<int>(fConnectedDevices.size());
}

static int find_device_by_name(const string &name) {
    for (int i = 0; i < fAvailablePeripherals.size(); i++) {
        // TODO check if it is better to test for equality or `starts_with`
        if (starts_with_ignore_case(fAvailablePeripherals[i]->identifier(), name)) {
//        if (devices[i].identifier().starts_with(name)) {
//        if (devices[i].identifier() == name) {
            console << "find device name >"
                    << " name:[" << fAvailablePeripherals[i]->identifier() << "]"
                    << " address:[" << fAvailablePeripherals[i]->address() << "]"
                    << " index:[" << i << "] "
                    << endl;
            return i;
        }
    }
    return NO_DEVICE_FOUND;
}

static int find_device_by_address(const string &address) {
    for (int i = 0; i < fAvailablePeripherals.size(); i++) {
        if (fAvailablePeripherals[i]->address() == address) {
            console << "finde device by address >"
                    << " name:[" << fAvailablePeripherals[i]->identifier() << "]"
                    << " address:[" << fAvailablePeripherals[i]->address() << "]"
                    << " index:[" << i << "] "
                    << endl;
            return i;
        }
    }
    return NO_DEVICE_FOUND;
}

static void failed_connection_type(const string &type) {
    console << "only the following connection types are allowed:" << endl;
    console << CONNECTION_TYPE_NAME << endl;
    console << CONNECTION_TYPE_ADDRESS << endl;
    console << CONNECTION_TYPE_INDEX << endl;
    console << "however, found connection type: " << endl;
    console << type << endl;
}

static bool compile_peripheral_indices(vector<int> &peripheral_indices,
                                       const string &type,
                                       const vector<string> &input) {
    if (type.starts_with(CONNECTION_TYPE_NAME)) {
        for (const string &t: input) {
            int mPeripheralIndex = find_device_by_name(t);
            if (mPeripheralIndex != NO_DEVICE_FOUND) {
                peripheral_indices.push_back(mPeripheralIndex);
            } else {
                console << "could not find name '" << t << "'." << endl;
            }
        }
        return !peripheral_indices.empty();
    } else if (type.starts_with(CONNECTION_TYPE_ADDRESS)) {
        for (const string &t: input) {
            int mPeripheralIndex = find_device_by_address(t);
            if (mPeripheralIndex != NO_DEVICE_FOUND) {
                peripheral_indices.push_back(mPeripheralIndex);
            } else {
                console << "could not find address '" << t << "'." << endl;
            }
        }
        return !peripheral_indices.empty();
    } else if (type.starts_with(CONNECTION_TYPE_INDEX)) {
        for (const string &t: input) {
            int mPeripheralIndex = NO_DEVICE_FOUND;
            try {
                mPeripheralIndex = stoi(t);
            } catch (const exception &e) {
                console << "could not convert index '" << t << "' to number: " << e.what() << endl;
            }
            if (mPeripheralIndex > NO_DEVICE_FOUND && mPeripheralIndex < fAvailablePeripherals.size()) {
                peripheral_indices.push_back(mPeripheralIndex);
            } else {
                console << "could not find index '" << t << "'." << endl;
            }
        }
        return !peripheral_indices.empty();
    }

    failed_connection_type(type);
    return false;
}

static vector<string> sanitize_device_identifiers(const vector<string> &input) {
    vector<string> mInput = vector<string>(input);
    for (string    &str: mInput) {
        str.erase(remove(str.begin(), str.end(), '\"'), str.end());
    }
    return mInput;
}

static bool connect_device_by_peripheral_index(int peripheral_index) {
    if (peripheral_index < 0 || peripheral_index >= fAvailablePeripherals.size()) {
        console
                << "could not find device with index "
                << peripheral_index
                << endl;
        return false;
    }
    auto mPeripheral = fAvailablePeripherals[peripheral_index];
    // TODO check if device is already connected
    if (mPeripheral->is_connected()) {
        console
                << "warning device '"
                << mPeripheral->identifier()
                << "' is already connected."
                << endl;
        return false;
    }
    /* test for supported characteristics */
    fCurrentConnectedDeviceIndex++;
    shared_ptr<Device> mDevice = make_shared<Device>(mPeripheral, fCurrentConnectedDeviceIndex);
    if (mDevice->has_supported_characteristics()) {
        fConnectedDevices.push_back(std::move(mDevice));
        console << "connected device '"
                << mPeripheral->identifier()
                << "' with 'connected device index' "
                << fCurrentConnectedDeviceIndex
                << endl;
        return true;
    } else {
        console
                << "warning device '"
                << mPeripheral->identifier()
                << "' has no supported services or characteristics.";
        console
                << " … disconnecting device"
                << endl;
        fCurrentConnectedDeviceIndex--;
        return false;
    }
}


const shared_ptr<Device> &get_null_device() {
    return null_device;
}

const shared_ptr<Device> &get_device(int connected_device_index) {
    if (connected_device_index < 0 || connected_device_index >= fConnectedDevices.size()) {
        console << "could not find device with index " << connected_device_index << endl;
        return null_device;
    }
    return fConnectedDevices[connected_device_index];
}

static bool isPeripheralAddress(const string &str) {
    // Check for 128-bit UUID format
    std::regex r1("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$");
    // Check for 16-bit or 32-bit UUID format
    std::regex r2("^0000[0-9a-fA-F]{2,4}-0000-1000-8000-00805f9b34fb$");
    return std::regex_match(str, r1) || std::regex_match(str, r2);
}

static bool isPeripheralName(const string &str) {
    return !str.empty() && !isPeripheralAddress(str);
}

int connect_device(int peripheral_index) {
    bool mSuccess = connect_device_by_peripheral_index(peripheral_index);
    if (mSuccess) {
        return fCurrentConnectedDeviceIndex;
    } else {
        return ERROR;
    }
}

int connect_device(std::string &name_or_UUID) {
    int mPeripheralIndex = NO_DEVICE_FOUND;
//    mPeripheralIndex = find_device_by_name(name_or_UUID);
//    if (mPeripheralIndex == NO_DEVICE_FOUND) {
//        mPeripheralIndex = find_device_by_address(name_or_UUID);
//    }
    if (isPeripheralAddress(name_or_UUID)) {
        mPeripheralIndex = find_device_by_address(name_or_UUID);
    } else if (isPeripheralName(name_or_UUID)) {
        mPeripheralIndex = find_device_by_name(name_or_UUID);
    }

    if (mPeripheralIndex == NO_DEVICE_FOUND) {
        error
                << "could not find device with name or UUID '"
                << name_or_UUID
                << "'"
                << endl;
        return ERROR;
    }

    bool mSuccess = connect_device_by_peripheral_index(mPeripheralIndex);
    if (mSuccess) {
        return fCurrentConnectedDeviceIndex;
    } else {
        return ERROR;
    }
}

static void handle_connect(const string &type, const vector<string> &input) {
    vector<int> mPeripheralIndices;
    bool        mSuccess = compile_peripheral_indices(mPeripheralIndices,
                                                      type,
                                                      sanitize_device_identifiers(input));

    if (!mSuccess) {
        console << "could not connect to any device" << endl;
        return;
    }

    console << "found "
            << mPeripheralIndices.size()
            << " device"
            << (mPeripheralIndices.size() > 1 ? "s" : "") << " to connect to."
            << endl;
    for (int i: mPeripheralIndices) {
        connect_device_by_peripheral_index(i);
    }
}

static void handle_disconnect(const string &type, const vector<string> &input) {
    // TODO implement disconnect: only accept 'connected device indices' not names and only single devices? adapt CLI
    console << "disconnect @todo" << endl;
    console << "type: " << type << endl;
    if (type == CONNECTION_TYPE_NAME || type == CONNECTION_TYPE_ADDRESS) {
        console << "connection type: " << type << " not yet implemented." << endl;
    } else if (type == CONNECTION_TYPE_INDEX) {
        if (input.empty()) {
            console << "device list is emprty" << endl;
        } else if (input.size() == 1) {
            try {
                int mConnectedDeviceIndex = stoi(input[0]);
                disconnect_device(mConnectedDeviceIndex);
            } catch (const exception &e) {
                console << "could not convert device index '" << input[0] << "' to number: " << e.what() << endl;
            }
        } else {
            console << "multiple disconnects not yet implemented." << endl;
        }
    } else {
        console << "connection type: " << type << " not supported" << endl;
    }
}

int disconnect_device(int connected_device_index) {
    // TODO there is still a problem with disconnecting devices. sometimes devices fail to disconnect and there sometimes devices try to disconnect twice.
    if (connected_device_index < 0 || connected_device_index >= fConnectedDevices.size()) {
        error
                << "could not find device with index "
                << connected_device_index
                << endl;
        return ERROR;
    }

    console
            << "attempting to disconnect from "
            << fConnectedDevices.size()
            << " device"
            << (fConnectedDevices.size() > 1 ? "s" : "")
            << endl;

    auto &mDevice = fConnectedDevices[connected_device_index];
    if (mDevice != nullptr) {
        int mDeviceIndex = ERROR;
        if (mDevice->is_connected()) {
            mDevice->disconnect();
            console << "disconnected device '"
                    << mDevice->name()
                    << "' with 'connected device index' "
                    << connected_device_index
                    << endl;
            mDeviceIndex = connected_device_index;
        } else {
            console << "warning device '"
                    << mDevice->name()
                    << "' is already disconnected."
                    << endl;
        }
        fConnectedDevices[connected_device_index] = nullptr;
        return mDeviceIndex;
    } else {
        console << "warning device ("
                << connected_device_index
                << ") is not intialized."
                << endl;
        return ERROR;
    }
}

static void print_device_capabilities() {
    for (const auto &device: fAvailablePeripherals) {
        try {
            cout << "------------------------------------------------------" << endl;
            string connectable_string = device->is_connectable() ? "connectable" : "non-connectable";
            string device_string      = device->identifier() + " [" + device->address() + "] ( " +
                                        to_string(device->rssi()) + " dBm )";
            cout << device_string << endl;
            cout << "    TX power    : " << dec << device->tx_power() << " dBm" << std::endl;
            cout << "    address type: " << device->address_type() << std::endl;
            cout << "                : " << connectable_string << endl;

//            cout << "MTU: " << device->mtu() << endl; // Maximum Transmission Unit
            if (device->is_connectable()) {
                const bool mIsConnected = device->is_connected();
                if (!mIsConnected) {
                    cout << "connect";
                    device->connect();
                    cout << "ed … successfully" << endl;
                } else {
                    cout << "already connected." << endl;
                }

                for (auto &service: device->services()) {
                    cout << "Service: " << service.uuid() << endl;

                    for (auto &characteristic: service.characteristics()) {
                        cout << "  Characteristic: " << characteristic.uuid() << endl;

                        cout << "    Capabilities: ";
                        for (auto &capability: characteristic.capabilities()) {
                            cout << capability << ", ";
                        }
                        cout << endl;

                        for (auto &descriptor: characteristic.descriptors()) {
                            cout << "    Descriptor: " << descriptor.uuid() << endl;
                        }
                    }
                }

                if (!mIsConnected) {
                    cout << "disconnect";
                    device->disconnect();
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

static void setup_watchdog() {
    fWatchdog = new Watchdog();
    fWatchdog->start();
    fWatchdog->set_frequency(fWatchdogFrequency);
}

static void clean_up_watchdog() {
    delete fWatchdog;
}


static void print_prompt() {
    cout << PROMPT_TOKE;
    cout.flush();
}


static bool parse_input(int argc, char *argv[]) {
    static const string CMD_CONNECT          = "connect";
    static const string CMD_CONNECT_CMD      = "" + CMD_CONNECT;
    static const string CMD_DISCONNECT       = "disconnect";
    static const string CMD_DISCONNECT_CMD   = "" + CMD_DISCONNECT;
    static const string CMD_DEVICES          = "devices";
    static const string CMD_DEVICES_CMD      = "d," + CMD_DEVICES;
    static const string CMD_READ             = "read";
    static const string CMD_WRITE            = "write";
    static const string CMD_VALUE            = "value";
    static const string DEFAULT_CONNECT_TYPE = CONNECTION_TYPE_NAME;
    try {
        cxxopts::Options commands("Sendungsbewusstsein", "sendungsbewusstsein broadcasts BLE devices via OSC.");
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
                        (CMD_DEVICES_CMD,
                         "specify devices for connect or disconnect. names that contain space need to be surrounded by quotation marks. indices might change after scan.",
                         cxxopts::value<vector<string>>())
                        (CMD_CONNECT_CMD,
                         "connect to device either by name (e.g '--connect=name -p \"WHOOP 4A0934182\" '), by address (e.g '--connect=address -p C6FBA-C7E8-0494-34C6-A54DF25AF596') or by index (e.g '--connect=index -p 3').",
                         cxxopts::value<string>()->implicit_value(DEFAULT_CONNECT_TYPE))
                        (CMD_DISCONNECT_CMD,
                         "disconnect from device either by name, address or index (see 'connect')",
                         cxxopts::value<string>()->implicit_value(DEFAULT_CONNECT_TYPE))
                        (CMD_READ,
                         "read characteristic from device, specified by index. ( e.g '-p \"WHOOP 4A0934182\" --read=3' )",
                         cxxopts::value<string>())
                        (CMD_WRITE,
                         "write characteristic to device, specified by index. multiple values must be written in quotation marks and separated by commas. ( e.g '-p \"WHOOP 4A0934182\" --write=2' )",
                         cxxopts::value<string>())
                        (CMD_VALUE,
                         R"(spedify values for writing characteristic. multiple values must be written in quotation marks and separated by commas. ( e.g '-p "WHOOP 4A0934182" --write=2 --value="0x0A,0x10,0x42"' ))",
                         cxxopts::value<string>());

        commands.add_options("OSC")
                ("w,watchdog",
                 "frequency of watchdog singals in milliseconds. a value of 0 turns the watchdog off.",
                 cxxopts::value<int>()->implicit_value(to_string(DEFAULT_WATCHDOG_SIGNAL_FREQUENCY_MS)))
                ("a,address", "OSC message transmit address",
                 cxxopts::value<std::string>()->implicit_value("127.0.0.1"))
                ("t,transmit", "port for transmitting messages", cxxopts::value<int>()->implicit_value("7000"))
                ("r,receive", "port for receiveing messages", cxxopts::value<int>()->implicit_value("7001"))
                ("m,multicast", "enable multicast broadcast", cxxopts::value<bool>()->implicit_value("false"));
        commands.parse_positional({CMD_DEVICES, ""});
        auto result = commands.parse(argc, argv);

        if (result.count("help")) {
            console << commands.help() << endl;
            return false;
        }

        if (result.count("info")) {
            console << "info" << endl;
            print_device_capabilities();
            return false;
        }

        if (result.count("quit")) {
            console << "quit" << endl;
            return true;
        }

        if (result.count("scan")) {
            // TODO add option to ignore devices without name--scan
            int mScanDurationMillis = result["scan"].as<int>(); // default: DEFAULT_SCAN_FOR_DEVICE_DURATION_MS
            scan_for_devices(mScanDurationMillis, true);
        }

        if (result.count("watchdog")) {
            console << "watchdog frequency = " << result["watchdog"].as<int>() << endl;
            fWatchdogFrequency = result["watchdog"].as<int>();
            if (fWatchdog != nullptr) {
                fWatchdog->set_frequency(fWatchdogFrequency);
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
        if (result.count(CMD_DEVICES)) {
            mPeripherals = result[CMD_DEVICES].as<vector<string >>();
            console << "handling devices ("
                    << mPeripherals.size()
                    << ") = {";
            for (const auto &s: mPeripherals) {
                console << s << ", ";
            }
            console << "}" << endl;
        }

        if ((result.count(CMD_CONNECT)
             || result.count(CMD_DISCONNECT)
             || result.count(CMD_READ)
             || result.count(CMD_WRITE))
            && mPeripherals.empty()) {
            error << "no device"
                  << (mPeripherals.size() > 1 ? "s " : " ")
                  << "specified to connect to, disconnect from, read from, or write to." << endl;
        } else {
            if (result.count(CMD_CONNECT)) {
                string mType = result[CMD_CONNECT].as<string>();
                handle_connect(mType, mPeripherals);
            } else if (result.count(CMD_DISCONNECT)) {
                string mType = result[CMD_DISCONNECT].as<string>();
                console << "type: " << mType << endl;
                handle_disconnect(mType, mPeripherals);
            }
            if (result.count(CMD_READ)) {
                console
                        << "TODO read characteristic "
                        << result[CMD_READ].as<string>()
                        << " from device(s)."
                        << endl;
            } else if (result.count(CMD_WRITE)) {
                if (result.count(CMD_VALUE)) {
                    console
                            << "TODO write characteristic "
                            << result[CMD_WRITE].as<string>()
                            << " with values ("
                            << result[CMD_VALUE].as<string>()
                            << ") to device(s) "
                            << endl;
                } else {
                    console
                            << "'write' requires values to be specified with '--value'."
                            << endl;
                }
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

static bool parse_input_vec(vector<string> &args_vec) {
    int         argc   = static_cast<int>(args_vec.size());
    char        **argv = new char *[args_vec.size()];
    for (size_t i      = 0; i < args_vec.size(); ++i) {
        argv[i] = new char[args_vec[i].length() + 1];
        strcpy(argv[i], args_vec[i].c_str());
    }

    bool mExit = parse_input(argc, argv);
    delete[] argv;
    return mExit;
}

static void osc_callback(string typetag, vector<any> message) {
    fCommandHandler->handle_message(typetag, message);
}

static void register_characteristics() {
    CharacteristicHeartRateMeasurment::register_characteristic();
    CharacteristicIndoorBikeData::register_characteristic();
    CharacteristicCyclingPowerMeasurement::register_characteristic();
    CharacteristicCyclingPowerControlPoint::register_characteristic();
    CharacteristicFitnessMachineControlPoint::register_characteristic();
}

static void setup_logging() {
    error.enable_output(true);
    error.set_output_stream(std::cerr);

    console.enable_output(true);
    console.set_output_stream(std::cout);
    console << "Sendungsbewusstsein" << endl;

#ifdef LOG_IN_FILE
    std::ofstream outputFile("output.txt");
    logger.set_output_stream(outputFile);
    logger.set_prefix("");
    logger << "This is a log message" << endl;
    logger << "This is another log message" << endl;
#endif
}

static void setup_OSC() {
    if (fAdapter == nullptr) {
        error << "no adapter available" << endl;
        return;
    }

    fCommandHandler = new TransceiverCommandHandler();
    Transceiver::init(default_application_properties.OSC_address.c_str(),
                      default_application_properties.OSC_transmit_port,
                      default_application_properties.OSC_receive_port,
                      default_application_properties.OSC_use_UDP_multicast);
    Transceiver::instance()->register_receive_callback(osc_callback);
    this_thread::sleep_for(std::chrono::seconds(1));
}

static void clean_up_OSC() {
    console
            << "shutting down OSC … ";
    Transceiver::instance()->finalize();
    console
            << "successfully"
            << endl;
    delete fCommandHandler;
}

static bool setup_BluetoothAdapter() {
    /* connect to adapter */
    auto adapter_optional = Utils::getAdapter();
    if (!adapter_optional.has_value()) {
        return false;
    }

    fAdapter = new Adapter(adapter_optional.value());
    console
            << "using adapter: "
            << fAdapter->identifier()
            << " [" << fAdapter->address() << "]"
            << endl;
    return true;
}

static void clean_up_BluetoothAdapter() {
    delete fAdapter;
}

#ifdef SB_RUN_AS_CLI

int main(int argc, char *argv[]) {
    bool mExit = false;
    setup_logging();
    register_characteristics();

    bool mHaveAdapter = setup_BluetoothAdapter();
    if (!mHaveAdapter || fAdapter == nullptr) {
        error << "could not find BLE adapter" << endl;
        return EXIT_FAILURE;
    }

#ifdef SB_HAS_CLI
    /* CLI args */
    if (argc > 1) {
        mExit = parse_input(argc, argv); /* parse CLI args */
    }
#endif

#ifdef SB_HAS_OSC
    /* OSC */
    setup_OSC();
#endif

#ifdef SB_HAS_WATCHDOG
    /* watchdog */
    setup_watchdog();
#endif

#ifdef SB_HAS_CLI
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

            mExit = parse_input_vec(commands);
        }
        print_prompt();
    }
#else
    string input;
    console << "press ENTER to exit ..." << endl;
    while (getline(cin, input)) { break; }
    console << "... done." << endl;
#endif

    console
            << "was connected to "
            << fConnectedDevices.size()
            << " device"
            << (fConnectedDevices.size() > 1 ? "s" : "")
            << "."
            << endl;

    reset_connected_devices();
#ifdef SB_HAS_WATCHDOG
    clean_up_watchdog();
#endif
#ifdef SB_HAS_OSC
    clean_up_OSC();
#endif
    clean_up_BluetoothAdapter();
    return EXIT_SUCCESS;
}

#else

void start_sendungsbewusstsein() {
    setup_logging();
    register_characteristics();

    bool mHaveAdapter = setup_BluetoothAdapter();
    if (!mHaveAdapter || fAdapter == nullptr) {
        error << "could not find BLE adapter" << endl;
    }
}

void finish_sendungsbewusstsein() {
    console
            << "was connected to "
            << fConnectedDevices.size()
            << " device"
            << (fConnectedDevices.size() > 1 ? "s" : "")
            << "."
            << endl;

    reset_connected_devices();
    clean_up_BluetoothAdapter();
}

int main(int argc, char *argv[]) {
    start_sendungsbewusstsein();
    scan_for_devices(5000, true, true);
    string mDevice = "WHOOP";
    connect_device(mDevice);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    finish_sendungsbewusstsein();
}

#endif

