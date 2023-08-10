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

    void scan_for_devices(int duration_in_milliseconds, int number_of_devices);

    void connect_device(int available_device_index,
                        int device_index,
                        int number_of_characteristics);

    void connect_device(string &name_or_UUID,
                        int device_index,
                        int number_of_characteristics);

    void disconnect_device(int device_index);

    void subscribe_to_characteristic(int device_index,
                                     int characteristic_index,
                                     int number_of_features);

    void unsubscribe_from_characteristic(int device_index,
                                         int characteristic_index);

    void get_device_name(int device_index,
                         string &device_name);

    void get_characteristic_name(int device_index,
                                 int characteristic_index,
                                 string &characteristic_name);

    void get_feature_name(int device_index,
                          int characteristic_index,
                          int feature_index,
                          string &feature_name);

    void get_feature_value(int device_index,
                           int characteristic_index,
                           int feature_index,
                           float value);

    void get_feature_value(int device_index,
                           int characteristic_index,
                           string &feature_name,
                           float value);

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

    void send_feature_with_value(int device_index,
                                 int characteristic_index,
                                 int feature_index,
                                 float value);

    void send_feature_with_value(int device_index,
                                 int characteristic_index,
                                 string &feature_name,
                                 float value);

    void send_watchdog(int interval_in_milliseconds);

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

#define TRANSCEIVER_CHECK_SOCKET_HEALTH  if (mTransmitSocket == nullptr) { return; }

    void send_i(const int message, const int a) {
        TRANSCEIVER_CHECK_SOCKET_HEALTH
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
          << message
          << a
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

    void send_ii(const int message, const int a, const int b) {
        TRANSCEIVER_CHECK_SOCKET_HEALTH
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
          << message
          << a
          << b
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

    void send_iii(const int message, const int a, int b, const int c) {
        TRANSCEIVER_CHECK_SOCKET_HEALTH
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
          << message
          << a
          << b
          << c
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

    void send_iiii(const int message, const int a, int b, const int c, const int d) {
        TRANSCEIVER_CHECK_SOCKET_HEALTH
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
          << message
          << a
          << b
          << c
          << d
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

    void send_iiif(const int message, const int a, int b, const int c, const float d) {
        TRANSCEIVER_CHECK_SOCKET_HEALTH
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
          << message
          << a
          << b
          << c
          << d
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

    void send_iisf(const int message, const int a, int b, string &c, const float d) {
        TRANSCEIVER_CHECK_SOCKET_HEALTH
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
          << message
          << a
          << b
          << c.c_str()
          << d
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

    void send_is(const int message, const int a, string &b) {
        TRANSCEIVER_CHECK_SOCKET_HEALTH
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
          << message
          << a
          << b.c_str()
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

    void send_iis(const int message, const int a, const int b, string &c) {
        TRANSCEIVER_CHECK_SOCKET_HEALTH
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
          << message
          << a
          << b
          << c.c_str()
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

    void send_iiis(const int message, const int a, const int b, const int c, string &d) {
        TRANSCEIVER_CHECK_SOCKET_HEALTH
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
          << message
          << a
          << b
          << c
          << d.c_str()
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

    void send_sii(const int message, string &a, const int b, const int c) {
        TRANSCEIVER_CHECK_SOCKET_HEALTH
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
          << message
          << a.c_str()
          << b
          << c
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

public:
    void register_receive_callback(CallbackType3_STRING_STRING_ANY callback) {
        pCallback = callback;
    }
};
