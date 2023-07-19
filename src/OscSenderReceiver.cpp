#include "Console.h"
#include "OscSenderReceiver.h"
#include "MOscPacketListener.h"

using namespace std;

OscSenderReceiver *OscSenderReceiver::fInstance = nullptr;

void OscSenderReceiver::osc_thread(const char *address,
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
            MOscPacketListener mOscListener(this);
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
            MOscPacketListener mOscListener(this);
            UdpListeningReceiveSocket s(IpEndpointName(IpEndpointName::ANY_ADDRESS, port_receive), &mOscListener);
            s.Run();
        }
    } catch (exception &e) {
        string mError = e.what();
        cerr << "@OSC / error: " << mError << endl;
    }
}