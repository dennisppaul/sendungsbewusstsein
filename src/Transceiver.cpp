#include "Console.h"
#include "Transceiver.h"
#include "OSCListener.h"
#include "SendungsbewusstseinDefines.h"

using namespace std;

Transceiver *Transceiver::fInstance = nullptr;

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

#define TRANSCEIVER_CHECK_SOCKET  if (mTransmitSocket == nullptr) { return; }

void Transceiver::send_watchdog(const int interval_in_milliseconds) {
    TRANSCEIVER_CHECK_SOCKET
    char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
    p << osc::BeginBundleImmediate
      << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
      << INFO_WATCHDOG
      << interval_in_milliseconds
      << osc::EndMessage
      << osc::EndBundle;
    mTransmitSocket->Send(p.Data(), p.Size());
}

void Transceiver::scan_for_devices(const int duration_in_milliseconds) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::connect_device(const int available_device_index) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::connect_device(string &name_or_UUID) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::disconnect_device(const int device_index) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::subscribe_to_characteristic(const int device_index,
                                              const int characteristic_index) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::unsubscribe_from_characteristic(const int device_index,
                                                  const int characteristic_index) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::get_device_name(const int device_index) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::get_characteristic_name(const int device_index,
                                          const int characteristic_index) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::get_feature_name(const int device_index,
                                   const int characteristic_index,
                                   const int feature_index) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::get_feature_value(const int device_index,
                                    const int characteristic_index,
                                    const int feature_index) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::get_feature_value(const int device_index,
                                    const int characteristic_index,
                                    string &feature_name) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::set_feature_value(const int device_index,
                                    const int characteristic_index,
                                    const int feature_index,
                                    const float value) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::set_feature_value(const int device_index,
                                    const int characteristic_index,
                                    string &feature_name,
                                    const float value) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::send_device_information(const int device_index,
                                          const int information) {
    TRANSCEIVER_CHECK_SOCKET

    char                      buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
    p << osc::BeginBundleImmediate
      << osc::BeginMessage(SB_OSC_ADDRESS_PATTERN)
      << INFO_DEVICE
      << device_index
      << information
      << osc::EndMessage
      << osc::EndBundle;
    mTransmitSocket->Send(p.Data(), p.Size());
}

void Transceiver::send_device_information_with_value(const int device_index,
                                                     const int information,
                                                     const int value) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::send_characteristic_information(const int device_index,
                                                  const int characteristic_index,
                                                  const int information) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::send_characteristic_information_with_value(const int device_index,
                                                             const int characteristic_index,
                                                             const int information,
                                                             const int value) {
    TRANSCEIVER_CHECK_SOCKET

}


void Transceiver::send_characteristic_feature_with_value(const int device_index,

                                                         const int characteristic_index,

                                                         const int feature_index,

                                                         const float value) {
    TRANSCEIVER_CHECK_SOCKET

}

void Transceiver::send_characteristic_feature_with_value(const int device_index,

                                                         const int characteristic_index,

                                                         string &feature_name,

                                                         const float value) {
    TRANSCEIVER_CHECK_SOCKET

}

