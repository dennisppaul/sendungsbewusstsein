#pragma once

#include <iostream>
#include <thread>

#include "ip/UdpSocket.h"
#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"

#include "Console.h"

#define DEBUG_OSC
#define DEFAULT_OSC_TRANSMIT_ADDRESS        "127.0.0.1"
#define DEFAULT_OSC_TRANSMIT_PORT           7000
#define DEFAULT_OSC_RECEIVE_PORT            7001
#define DEFAULT_USE_UDP_MULTICAST           false

using namespace std;

class OscSenderReceiver {
public:

    static OscSenderReceiver *init(const char *address, int port_transmit, int port_receive, bool use_muilticast) {
        if (fInstance == nullptr) {
            console << "@OSC initializing OSC with user values" << endl;
            fInstance = new OscSenderReceiver(address, port_transmit, port_receive, use_muilticast);
        }
        return fInstance;
    }

    static OscSenderReceiver *instance() {
        if (fInstance == nullptr) {
            console << "@OSC starting OSC with default values" << endl;
            fInstance = new OscSenderReceiver(DEFAULT_OSC_TRANSMIT_ADDRESS,
                                              DEFAULT_OSC_TRANSMIT_PORT,
                                              DEFAULT_OSC_RECEIVE_PORT,
                                              DEFAULT_USE_UDP_MULTICAST);
        }
        return fInstance;
    }

    //    OscSenderReceiver(const OscManager &copy) : mOSCThread(copy.mOSCThread), mTransmitSocket(copy.mTransmitSocket) {}

    OscSenderReceiver(const OscSenderReceiver &) = delete;

    OscSenderReceiver &operator=(const OscSenderReceiver &) = delete;

    void finalize() {
        mOSCThread->detach();
        delete mOSCThread;
        delete mTransmitSocket;
    }

    void send(const string &address_pattern, const float value) { // watchdog
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

    void send(const int ID,
              const char* command,
              const char* name) {
        if (mTransmitSocket != nullptr) {
            char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
            osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
            string                    mMessageAddrPattern = string(OSC_MSG_DELIMITER) +
                                                            string(OSC_MSG) +
                                                            string(OSC_MSG_DELIMITER);
            p << osc::BeginBundleImmediate
              << osc::BeginMessage(mMessageAddrPattern.c_str())
              << ID
              << command
              << name
              << osc::EndMessage
              << osc::EndBundle;
            mTransmitSocket->Send(p.Data(), p.Size());
        }
    }

    void send(const int ID,
              const string &characteristic,
              const float value) {
        if (mTransmitSocket != nullptr) {
            char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
            osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
            string                    mMessageAddrPattern = string(OSC_MSG_DELIMITER) +
                                                            string(OSC_MSG) +
                                                            string(OSC_MSG_DELIMITER);
            p << osc::BeginBundleImmediate
              << osc::BeginMessage(mMessageAddrPattern.c_str())
              << ID
              << characteristic.c_str()
              << value
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
    static OscSenderReceiver    *fInstance;
    static const constexpr char *OSC_MSG                        = ("sendungsbewusstsein");
    static const constexpr char *OSC_MSG_DELIMITER              = ("/");
    static const uint16_t       OSC_TRANSMIT_OUTPUT_BUFFER_SIZE = 1024;
    thread                      *mOSCThread                     = nullptr;
    UdpTransmitSocket           *mTransmitSocket                = nullptr;
    bool                        fUseMulticast;

    OscSenderReceiver(const char *address,
                      int port_transmit,
                      int port_receive,
                      bool use_muilticast) :
            fUseMulticast(use_muilticast) {
        initialize_transmit_socket(address, port_transmit);
        mOSCThread = new thread(&OscSenderReceiver::osc_thread,
                                this,
                                address,
                                port_transmit,
                                port_receive,
                                use_muilticast);
    }

    static bool addr_pattern_equals(const osc::ReceivedMessage &msg, const char *pAddrPatter) {
        return (strcmp(msg.AddressPattern(), pAddrPatter) == 0);
    }

    void osc_thread(const char *address,
                    int port_transmit,
                    int port_receive,
                    bool use_muilticast);

    void initialize_transmit_socket(const char *address, int port_transmit) {
        mTransmitSocket = new UdpTransmitSocket(IpEndpointName(address, port_transmit));
    }
};
