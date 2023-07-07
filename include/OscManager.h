#pragma once

#include <thread>

#include "ip/UdpSocket.h"
#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"

#define OSC_TRANSMIT_ADDRESS        "127.0.0.1"
#define OSC_TRANSMIT_PORT           7000
#define OSC_RECEIVE_PORT            7001

using namespace std;

class OscManager {

    thread mOSCThread;
    UdpTransmitSocket *mTransmitSocket = nullptr;
    const uint16_t OSC_TRANSMIT_OUTPUT_BUFFER_SIZE = 1024;

public:
    OscManager() {
        mOSCThread = thread(&OscManager::osc_thread, this);
        IpEndpointName mEndpointName = IpEndpointName(OSC_TRANSMIT_ADDRESS, OSC_TRANSMIT_PORT);
        mTransmitSocket = new UdpTransmitSocket(mEndpointName);
    }

    void finalize() {
        mOSCThread.detach();
    }

    void send(float value) {
        static const char OSC_MSG[] = "/sendungsbewusstsein";

        char buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(OSC_MSG)
          << value
          << osc::EndMessage
          << osc::EndBundle;
        mTransmitSocket->Send(p.Data(), p.Size());
    }

   void process(const osc::ReceivedMessage &msg) {
//        osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
//        int                                mSerialPort;
//        int                                mSerialData;
//        args >> mSerialPort >> mSerialData >> osc::EndMessage;

        uint8_t mData[msg.ArgumentCount()];
        uint8_t i = 0;
        for (osc::ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
             arg != msg.ArgumentsEnd(); ++arg) {
            mData[i] = (uint8_t) arg->AsInt32();
            i++;
        }
//        data_receive(ALL_PERIPHERALS, mData, msg.ArgumentCount());
    }

private:

    static bool addr_pattern_equals(const osc::ReceivedMessage &msg, const char *pAddrPatter) {
        return (strcmp(msg.AddressPattern(), pAddrPatter) == 0);
    }

    void osc_thread();
};

