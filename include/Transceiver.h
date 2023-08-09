#pragma once

#include <iostream>
#include <thread>
#include <any>
#include <utility>
#include <vector>
#include <string>

#include "ip/UdpSocket.h"
#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"

#include "Console.h"
#include "StringUtils.h"

#define DEBUG_OSC
#define DEFAULT_OSC_TRANSMIT_ADDRESS        "127.0.0.1"
#define DEFAULT_OSC_TRANSMIT_PORT           7000
#define DEFAULT_OSC_RECEIVE_PORT            7001
#define DEFAULT_USE_UDP_MULTICAST           false

using namespace std;

class Transceiver {
public:
    static Transceiver *init(const char *address, int port_transmit, int port_receive, bool use_muilticast) {
        if (fInstance == nullptr) {
            console << "@OSC initializing OSC with user values" << endl;
            fInstance = new Transceiver(address, port_transmit, port_receive, use_muilticast);
        }
        return fInstance;
    }

    static Transceiver *instance() {
        if (fInstance == nullptr) {
            console << "@OSC starting OSC with default values" << endl;
            fInstance = new Transceiver(DEFAULT_OSC_TRANSMIT_ADDRESS,
                                        DEFAULT_OSC_TRANSMIT_PORT,
                                        DEFAULT_OSC_RECEIVE_PORT,
                                        DEFAULT_USE_UDP_MULTICAST);
        }
        return fInstance;
    }

    Transceiver(const Transceiver &) = delete;

    Transceiver &operator=(const Transceiver &) = delete;

    void finalize() {
        mOSCThread->detach();
        delete mOSCThread;
        delete mTransmitSocket;
    }

    void send_watchdog(int interval_in_milliseconds);

    void scan_for_devices(int duration_in_milliseconds);

    void connect_device(int available_device_index);

    void connect_device(string &name_or_UUID);

    void disconnect_device(int device_index);

    void subscribe_to_characteristic(int device_index,
                                     int characteristic_index);

    void unsubscribe_from_characteristic(int device_index,
                                         int characteristic_index);

    void get_device_name(int device_index);

    void get_characteristic_name(int device_index,
                                 int characteristic_index);

    void get_feature_name(int device_index,
                          int characteristic_index,
                          int feature_index);

    void get_feature_value(int device_index,
                           int characteristic_index,
                           int feature_index);

    void get_feature_value(int device_index,
                           int characteristic_index,
                           string &feature_name);

    void set_feature_value(int device_index,
                           int characteristic_index,
                           int feature_index,
                           float value);

    void set_feature_value(int device_index,
                           int characteristic_index,
                           string &feature_name,
                           float value);

    void send_device_information(int device_index,
                                 int information);

    void send_device_information_with_value(int device_index,
                                            int information,
                                            int value);

    void send_characteristic_information(int device_index,
                                         int characteristic_index,
                                         int information);

    void send_characteristic_information_with_value(int device_index,
                                                    int characteristic_index,
                                                    int information,
                                                    int value);

    void send_characteristic_feature_with_value(int device_index,

                                                int characteristic_index,

                                                int feature_index,

                                                float value);

    void send_characteristic_feature_with_value(int device_index,

                                                int characteristic_index,

                                                string &feature_name,

                                                float value);

//    void send_device_information_with_value(const int connected_device_index,
//                                            const char *information,
//                                            const int value) {
//        if (mTransmitSocket != nullptr) {
//            char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
//            osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
//            string                    mMessageAddrPattern = string(SB_OSC_ADDRESS_PATTERN_DELIMITER) +
//                                                            string(SB_OSC_ADDRESS_PATTERN) +
//                                                            string(SB_OSC_ADDRESS_PATTERN_DELIMITER);
//            p << osc::BeginBundleImmediate
//              << osc::BeginMessage(mMessageAddrPattern.c_str())
//              <<
//              << connected_device_index
//              << information
//              << value
//              << osc::EndMessage
//              << osc::EndBundle;
//            mTransmitSocket->Send(p.Data(), p.Size());
//        }
//    }

//    void send_characteristic_command(const int connected_device_index,
//                                     const string &characteristic,
//                                     const int index) {
//        if (mTransmitSocket != nullptr) {
//            char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
//            osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
//            string                    mMessageAddrPattern = string(SB_OSC_ADDRESS_PATTERN_DELIMITER) +
//                                                            string(SB_OSC_ADDRESS_PATTERN) +
//                                                            string(SB_OSC_ADDRESS_PATTERN_DELIMITER);
//            p << osc::BeginBundleImmediate
//              << osc::BeginMessage(mMessageAddrPattern.c_str())
//              << connected_device_index
//              << characteristic.c_str()
//              << index
//              << osc::EndMessage
//              << osc::EndBundle;
//            mTransmitSocket->Send(p.Data(), p.Size());
//        }
//    }

//    void send_characteristic_command(const int connected_device_index,
//                                     const string &command,
//                                     const string &characteristic,
//                                     const int supported_characteristic_index) {
//        if (mTransmitSocket != nullptr) {
//            char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
//            osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
//            string                    mMessageAddrPattern = string(SB_OSC_ADDRESS_PATTERN_DELIMITER) +
//                                                            string(SB_OSC_ADDRESS_PATTERN) +
//                                                            string(SB_OSC_ADDRESS_PATTERN_DELIMITER);
//            p << osc::BeginBundleImmediate
//              << osc::BeginMessage(mMessageAddrPattern.c_str())
//              << connected_device_index
//              << command.c_str()
//              << characteristic.c_str()
//              << supported_characteristic_index
//              << osc::EndMessage
//              << osc::EndBundle;
//            mTransmitSocket->Send(p.Data(), p.Size());
//        }
//    }

//    void send_characteristic_feature_with_value(const int connected_device_index,
//                                                const string &characteristic,
//                                                const string &feature,
//                                                const float value) {
//        if (mTransmitSocket != nullptr) {
//            char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
//            osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
//            string                    mMessageAddrPattern = string(SB_OSC_ADDRESS_PATTERN_DELIMITER) +
//                                                            string(SB_OSC_ADDRESS_PATTERN) +
//                                                            string(SB_OSC_ADDRESS_PATTERN_DELIMITER);
//            p << osc::BeginBundleImmediate
//              << osc::BeginMessage(mMessageAddrPattern.c_str())
//              << connected_device_index
//              << characteristic.c_str()
//              << feature.c_str()
//              << value
//              << osc::EndMessage
//              << osc::EndBundle;
//            mTransmitSocket->Send(p.Data(), p.Size());
//        }
//    }

    void process(const osc::ReceivedMessage &msg) {
        std::string mAddressPattern = msg.AddressPattern();
        if (mAddressPattern.starts_with(SB_OSC_ADDRESS_PATTERN)) {}
        std::vector<std::any>                     message;
        for (osc::ReceivedMessage::const_iterator arg = msg.ArgumentsBegin(); arg != msg.ArgumentsEnd(); ++arg) {
            if (arg->IsString()) {
                message.emplace_back(arg->AsString());
            } else if (arg->IsFloat()) {
                message.emplace_back(arg->AsFloat());
            } else if (arg->IsDouble()) {
                message.emplace_back(arg->AsDouble());
            } else if (arg->IsInt32()) {
                message.emplace_back(arg->AsInt32());
            } else if (arg->IsBool()) {
                message.emplace_back(arg->AsBool());
            } else if (arg->IsChar()) {
                message.emplace_back(arg->AsChar());
            } else {
                console
                        << "unknown type in OSC message. "
                        << "only supporting"
                        << "string, "
                        << "float, "
                        << "double, "
                        << "int32, "
                        << "bool, "
                        << "char."
                        << endl;
            }
        }
        callback_receive(msg.TypeTags(), message);
    }

private:
    typedef void (*CallbackType3_STRING_STRING_ANY)(std::string, std::vector<std::any>);

    static Transceiver              *fInstance;
    static const constexpr char     *SB_OSC_ADDRESS_PATTERN         = "/sendungsbewusstsein/";
    static const uint16_t           OSC_TRANSMIT_OUTPUT_BUFFER_SIZE = 1024;
    thread                          *mOSCThread                     = nullptr;
    UdpTransmitSocket               *mTransmitSocket                = nullptr;
    CallbackType3_STRING_STRING_ANY pCallback                       = nullptr;

    void callback_receive(std::string typetag, std::vector<std::any> message) {
        if (pCallback) {
            pCallback(std::move(typetag), std::move(message));
        }
    }

    Transceiver(const char *address,
                int port_transmit,
                int port_receive,
                bool use_muilticast) {
        initialize_transmit_socket(address, port_transmit);
        mOSCThread = new thread(&Transceiver::osc_thread,
                                this,
                                address,
                                port_transmit,
                                port_receive,
                                use_muilticast);
    }

    void osc_thread(const char *address,
                    int port_transmit,
                    int port_receive,
                    bool use_muilticast);

    void initialize_transmit_socket(const char *address, int port_transmit) {
        mTransmitSocket = new UdpTransmitSocket(IpEndpointName(address, port_transmit));
    }

public:
    void register_receive_callback(CallbackType3_STRING_STRING_ANY callback) {
        pCallback = callback;
    }
};
