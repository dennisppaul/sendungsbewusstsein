#pragma once

#include <iostream>
#include <thread>

#include "ip/UdpSocket.h"
#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"

#define OSC_TRANSMIT_ADDRESS        "127.0.0.1"
#define OSC_TRANSMIT_PORT           7000
#define OSC_RECEIVE_PORT            7001
//#define USE_UDP_MULTICAST

using namespace std;

class OscManager {
public:
    OscManager() {
        mOSCThread                   = thread(&OscManager::osc_thread, this);
        IpEndpointName mEndpointName = IpEndpointName(OSC_TRANSMIT_ADDRESS, OSC_TRANSMIT_PORT);
        mTransmitSocket = new UdpTransmitSocket(mEndpointName);
    }

//    OscManager(const OscManager &copy) {}

    void finalize() {
        mOSCThread.detach();
    }

    void send(const string address_pattern, const float value) const {
        char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        std::string               mMessageAddrPattern = string(OSC_MSG_DELIMITER) +
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

    void process(const osc::ReceivedMessage &msg) const {
//        osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
//        int                                mSerialPort;
//        int                                mSerialData;
//        args >> mSerialPort >> mSerialData >> osc::EndMessage;

        uint8_t                                   mData[msg.ArgumentCount()];
        uint8_t                                   i   = 0;
        for (osc::ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
             arg != msg.ArgumentsEnd(); ++arg) {
            mData[i] = (uint8_t) arg->AsInt32();
            cout << "received: " << mData[i] << endl;
            i++;
        }
//        receive(mData, msg.ArgumentCount());
    }

private:
    static const constexpr char *OSC_MSG           = ("sendungsbewusstsein");
    static const constexpr char *OSC_MSG_DELIMITER = ("/");
    thread            mOSCThread;
    UdpTransmitSocket *mTransmitSocket                = nullptr;
    const uint16_t    OSC_TRANSMIT_OUTPUT_BUFFER_SIZE = 1024;

    static bool addr_pattern_equals(const osc::ReceivedMessage &msg, const char *pAddrPatter) {
        return (strcmp(msg.AddressPattern(), pAddrPatter) == 0);
    }

    void osc_thread();
};

