#include "OscManager.h"
#include "MOscPacketListener.h"

using namespace std;

void OscManager::osc_thread() {
    cout << "+++ @OSC / start listening on " << OSC_TRANSMIT_ADDRESS << ":" << OSC_RECEIVE_PORT << endl;

    try {
//#define USE_UDP_MULTICAST
#ifdef USE_UDP_MULTICAST
        MOscPacketListener mOscListener;
            PacketListener *listener_ = &mOscListener;
            IpEndpointName mIpEndpointName = IpEndpointName(KLANG_OSC_TRANSMIT_ADDRESS, KLANG_OSC_RECEIVE_PORT);
            if (mIpEndpointName.IsMulticastAddress()) {
#ifdef DEBUG_OSC
                KLANG_LOG("@klangstrom_arduino using UDP Multicast");
#endif
                UdpSocket mUdpSocket;
                mUdpSocket.SetAllowReuse(true);
                mUdpSocket.Bind(mIpEndpointName);
                SocketReceiveMultiplexer mux_;
                mux_.AttachSocketListener(&mUdpSocket, listener_);
                mux_.Run();
            } else {
                UdpListeningReceiveSocket s(IpEndpointName(IpEndpointName::ANY_ADDRESS, KLANG_OSC_RECEIVE_PORT),
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
//            KLANG_LOG("@klangstrom_arduino osc_thread %s", mError.c_str());
    }
}