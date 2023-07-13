#include "OscSenderReceiver.h"
#include "MOscPacketListener.h"

using namespace std;

void OscSenderReceiver::osc_thread() {
    cout << "+++ @OSC / start listening on " << OSC_TRANSMIT_ADDRESS << ":" << OSC_RECEIVE_PORT << endl;

    try {
#ifdef USE_UDP_MULTICAST
        MOscPacketListener mOscListener(this);
            PacketListener *listener_ = &mOscListener;
            IpEndpointName mIpEndpointName = IpEndpointName(OSC_TRANSMIT_ADDRESS, OSC_RECEIVE_PORT);
            if (mIpEndpointName.IsMulticastAddress()) {
#ifdef DEBUG_OSC
                LOG("@klangstrom_arduino using UDP Multicast");
#endif
                UdpSocket mUdpSocket;
                mUdpSocket.SetAllowReuse(true);
                mUdpSocket.Bind(mIpEndpointName);
                SocketReceiveMultiplexer mux_;
                mux_.AttachSocketListener(&mUdpSocket, listener_);
                mux_.Run();
            } else {
                UdpListeningReceiveSocket s(IpEndpointName(IpEndpointName::ANY_ADDRESS, OSC_RECEIVE_PORT),
                                            &mOscListener);
                s.Run();
            }
#else
        MOscPacketListener mOscListener(this);
        UdpListeningReceiveSocket s(IpEndpointName(IpEndpointName::ANY_ADDRESS, OSC_RECEIVE_PORT), &mOscListener);
        s.Run();
#endif
    } catch (exception &e) {
        string mError = e.what();
        cerr << "+++ @OSC / error: " << mError << endl;
    }
}