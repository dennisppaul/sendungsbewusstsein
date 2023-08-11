#include "Transceiver.h"

#ifdef SB_HAS_OSC

#include <iostream>
#include "ip/UdpSocket.h"
#include "osc/OscPacketListener.h"
#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"

using namespace std;

void process(Transceiver *transceiver, const osc::ReceivedMessage &msg) {
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
    transceiver->callback_receive(msg.TypeTags(), message);
}


class OSCListener : public osc::OscPacketListener {
public:
    explicit OSCListener(Transceiver *manager) : fManager(manager) {}

private:
    Transceiver *fManager;
protected:
    void ProcessMessage(const osc::ReceivedMessage &msg, const IpEndpointName &remoteEndpoint) override {
        (void) remoteEndpoint;  // suppress unused parameter warning
        try {
            process(fManager, msg);
        } catch (osc::Exception &e) {
#ifdef DEBUG_OSC
            cerr << "@OSC error in OSC listener: " << msg.AddressPattern() << "" << e.what();
#endif
        }
    }
};

void Transceiver::osc_thread(const char *address,
                             int port_transmit,
                             int port_receive,
                             bool use_muilticast) {
    console << "@OSC start transmitting on "
            << address << ":" << port_transmit
            << " + receiving on :"
            << port_receive
            << endl;

    try {
        if (use_muilticast) {
            OSCListener mOscListener(this);
            PacketListener *listener_      = &mOscListener;
            IpEndpointName mIpEndpointName = IpEndpointName(address, port_receive);
            if (mIpEndpointName.IsMulticastAddress()) {
#ifdef DEBUG_OSC
                console << "@OSC using UDP Multicast" << endl;
#endif
                UdpSocket mUdpSocket;
                mUdpSocket.SetAllowReuse(true);
                mUdpSocket.Bind(mIpEndpointName);
                SocketReceiveMultiplexer mux_;
                mux_.AttachSocketListener(&mUdpSocket, listener_);
                mux_.Run();
            } else {
                UdpListeningReceiveSocket s(IpEndpointName(IpEndpointName::ANY_ADDRESS, port_receive),
                                            &mOscListener);
                s.Run();
            }
        } else {
            OSCListener mOscListener(this);
            UdpListeningReceiveSocket s(IpEndpointName(IpEndpointName::ANY_ADDRESS, port_receive), &mOscListener);
            s.Run();
        }
    } catch (exception &e) {
        string mError = e.what();
        cerr << "@OSC / error: " << mError << endl;
    }
}

static UdpTransmitSocket *mTransmitSocket = nullptr;

#define TRANSCEIVER_CHECK_SOCKET_HEALTH  if (mTransmitSocket == nullptr) { return; }

void Transceiver::callback_receive(std::string typetag, std::vector<std::any> message) {
    if (pCallback) {
        pCallback(std::move(typetag), std::move(message));
    }
}

Transceiver::Transceiver(const char *address,
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

void Transceiver::initialize_transmit_socket(const char *address, int port_transmit) {
    mTransmitSocket = new UdpTransmitSocket(IpEndpointName(address, port_transmit));
}

void Transceiver::send_i(const int message, const int a) {
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

void Transceiver::send_ii(const int message, const int a, const int b) {
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

void Transceiver::send_iii(const int message, const int a, int b, const int c) {
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

void Transceiver::send_iiii(const int message, const int a, int b, const int c, const int d) {
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

void Transceiver::send_iiif(const int message, const int a, int b, const int c, const float d) {
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

void Transceiver::send_iisf(const int message, const int a, int b, string &c, const float d) {
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

void Transceiver::send_is(const int message, const int a, string &b) {
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

void Transceiver::send_iis(const int message, const int a, const int b, string &c) {
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

void Transceiver::send_iiis(const int message, const int a, const int b, const int c, string &d) {
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

void Transceiver::send_sii(const int message, string &a, const int b, const int c) {
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


#endif