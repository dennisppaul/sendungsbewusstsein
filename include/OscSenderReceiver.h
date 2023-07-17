#pragma once

#include <iostream>
#include <thread>

#include "ip/UdpSocket.h"
#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"

#define OSC_TRANSMIT_ADDRESS        "127.0.0.1"
#define OSC_RECEIVE_PORT            7001
#define OSC_TRANSMIT_PORT           7000
//#define USE_UDP_MULTICAST

using namespace std;

class OscSenderReceiver {
public:
    static OscSenderReceiver &instance() {
        static OscSenderReceiver instance;
        return instance;
    }

    //    OscSenderReceiver(const OscManager &copy) : mOSCThread(copy.mOSCThread), mTransmitSocket(copy.mTransmitSocket) {}

    OscSenderReceiver(const OscSenderReceiver &) = delete;

    OscSenderReceiver &operator=(const OscSenderReceiver &) = delete;

    void finalize() {
        mOSCThread->detach();
        delete mOSCThread;
        delete mTransmitSocket;
    }

    void send(const string &address_pattern, const float value) {
//        if (mTransmitSocket == nullptr) {
//            cerr << "why for the love of it all is `mTransmitSocket` a `nullptr` twice?!?" << endl;
//            initialize_transmit_socket();
//        }
        if (mTransmitSocket != nullptr) {
            char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
            osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
            string                    mMessageAddrPattern = string(OSC_MSG_DELIMITER) +
                                                            string(OSC_MSG) +
                                                            string(OSC_MSG_DELIMITER) +
                                                            address_pattern;
            p << osc::BeginBundleImmediate
              << osc::BeginMessage(mMessageAddrPattern.c_str())
              << value
              << osc::EndMessage
              << osc::EndBundle;
            mTransmitSocket->Send(p.Data(), p.Size());
        }
    }

    void send(const string &address_pattern, const int ID, const float value) {
        if (mTransmitSocket != nullptr) {
            char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
            osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
            string                    mMessageAddrPattern = string(OSC_MSG_DELIMITER) +
                                                            string(OSC_MSG) +
                                                            string(OSC_MSG_DELIMITER) +
                                                            address_pattern;
            p << osc::BeginBundleImmediate
              << osc::BeginMessage(mMessageAddrPattern.c_str())
              << ID
              << value
              << osc::EndMessage
              << osc::EndBundle;
            mTransmitSocket->Send(p.Data(), p.Size());
        }
    }

    void send(const string &address_pattern,
              const int ID,
              const char *command,
              const float value) {
        if (mTransmitSocket != nullptr) {
            char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
            osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
            string                    mMessageAddrPattern = string(OSC_MSG_DELIMITER) +
                                                            string(OSC_MSG) +
                                                            string(OSC_MSG_DELIMITER) +
                                                            address_pattern;
            p << osc::BeginBundleImmediate
              << osc::BeginMessage(mMessageAddrPattern.c_str())
              << ID
              << command
              << value
              << osc::EndMessage
              << osc::EndBundle;
            mTransmitSocket->Send(p.Data(), p.Size());
        }
    }

    void send(const string &address_pattern,
              const int ID,
              const char *command) {
        if (mTransmitSocket != nullptr) {
            char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
            osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
            string                    mMessageAddrPattern = string(OSC_MSG_DELIMITER) +
                                                            string(OSC_MSG) +
                                                            string(OSC_MSG_DELIMITER) +
                                                            address_pattern;
            p << osc::BeginBundleImmediate
              << osc::BeginMessage(mMessageAddrPattern.c_str())
              << ID
              << command
              << osc::EndMessage
              << osc::EndBundle;
            mTransmitSocket->Send(p.Data(), p.Size());
        }
    }

    void process(const osc::ReceivedMessage &msg) {
//        osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
//        int                                mSerialPort;
//        int                                mSerialData;
//        args >> mSerialPort >> mSerialData >> osc::EndMessage;

        uint8_t                                   mData[msg.ArgumentCount()];
        uint8_t                                   i   = 0;
        for (osc::ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
             arg != msg.ArgumentsEnd(); ++arg) {
            mData[i] = (uint8_t) arg->AsInt32();
            cout << "received: " << (int) mData[i] << endl;
            i++;
        }
//        receive(mData, msg.ArgumentCount());
    }

private:
    static const constexpr char *OSC_MSG                        = ("sendungsbewusstsein");
    static const constexpr char *OSC_MSG_DELIMITER              = ("/");
    static const uint16_t       OSC_TRANSMIT_OUTPUT_BUFFER_SIZE = 1024;
    thread                      *mOSCThread                     = nullptr;
    UdpTransmitSocket           *mTransmitSocket                = nullptr;

    OscSenderReceiver() {
        initialize_transmit_socket();
        mOSCThread = new thread(&OscSenderReceiver::osc_thread, this);
    }

    static bool addr_pattern_equals(const osc::ReceivedMessage &msg, const char *pAddrPatter) {
        return (strcmp(msg.AddressPattern(), pAddrPatter) == 0);
    }

    void osc_thread();

    void initialize_transmit_socket() {
        // TODO why for the love of it all is `mTransmitSocket` a `nullptr` twice?!?
        mTransmitSocket = new UdpTransmitSocket(IpEndpointName(OSC_TRANSMIT_ADDRESS, OSC_TRANSMIT_PORT));
    }
};

