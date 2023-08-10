#include "Console.h"
#include "Transceiver.h"
#include "OSCListener.h"
#include "Sendungsbewusstsein.h"

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

void Transceiver::scan_for_devices(const int duration_in_milliseconds,
                                   const int number_of_devices) {
    send_ii(CMD_SCAN_FOR_DEVICES,
            duration_in_milliseconds,
            number_of_devices);
}

void Transceiver::connect_device(const int available_device_index,
                                 const int device_index,
                                 const int number_of_characteristics) {
    send_iii(CMD_CONNECT_DEVICE,
             available_device_index,
             device_index,
             number_of_characteristics);
}

void Transceiver::connect_device(string &name_or_UUID,
                                 const int device_index,
                                 const int number_of_characteristics) {
    send_sii(CMD_CONNECT_DEVICE,
             name_or_UUID,
             device_index,
             number_of_characteristics);
}

void Transceiver::disconnect_device(const int device_index) {
    send_i(CMD_DISCONNECT_DEVICE,
           device_index);
}

void Transceiver::subscribe_to_characteristic(const int device_index,
                                              const int characteristic_index,
                                              const int number_of_features) {
    send_iii(CMD_SUBSCRIBE_TO_CHARACTERISTIC,
             device_index,
             characteristic_index,
             number_of_features);
}

void Transceiver::unsubscribe_from_characteristic(const int device_index,
                                                  const int characteristic_index) {
    send_ii(CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC,
            device_index,
            characteristic_index);
}

void Transceiver::get_device_name(const int device_index, string &device_name) {
    send_is(CMD_GET_DEVICE_NAME,
            device_index,
            device_name);
}

void Transceiver::get_characteristic_name(const int device_index,
                                          const int characteristic_index,
                                          string &characteristic_name) {
    send_iis(CMD_GET_CHARACTERISTIC_NAME,
             device_index,
             characteristic_index,
             characteristic_name);
}

void Transceiver::get_feature_name(const int device_index,
                                   const int characteristic_index,
                                   const int feature_index,
                                   string &feature_name) {
    send_iiis(CMD_GET_FEATURE_NAME,
              device_index,
              characteristic_index,
              feature_index,
              feature_name);
}

void Transceiver::get_feature_value(const int device_index,
                                    const int characteristic_index,
                                    const int feature_index,
                                    const float value) {
    send_iiif(CMD_GET_FEATURE_VALUE,
              device_index,
              characteristic_index,
              feature_index,
              value);
}

void Transceiver::get_feature_value(const int device_index,
                                    const int characteristic_index,
                                    string &feature_name,
                                    const float value) {
    send_iisf(CMD_GET_FEATURE_VALUE,
              device_index,
              characteristic_index,
              feature_name,
              value);
}

void Transceiver::set_feature_value(const int device_index,
                                    const int characteristic_index,
                                    const int feature_index,
                                    const float value) {
    send_iiif(CMD_SET_FEATURE_VALUE,
              device_index,
              characteristic_index,
              feature_index,
              value);
}

void Transceiver::set_feature_value(const int device_index,
                                    const int characteristic_index,
                                    string &feature_name,
                                    const float value) {
    send_iisf(CMD_SET_FEATURE_VALUE,
              device_index,
              characteristic_index,
              feature_name,
              value);
}

void Transceiver::send_device_information(const int device_index,
                                          const int information) {
    send_ii(INFO_DEVICE,
            device_index,
            information);
}

void Transceiver::send_device_information_with_value(const int device_index,
                                                     const int information,
                                                     const int value) {
    send_iii(INFO_DEVICE,
             device_index,
             information,
             value);
}

void Transceiver::send_characteristic_information(const int device_index,
                                                  const int characteristic_index,
                                                  const int information) {
    send_iii(INFO_CHARACTERISTIC,
             device_index,
             characteristic_index,
             information);
}

void Transceiver::send_characteristic_information_with_value(const int device_index,
                                                             const int characteristic_index,
                                                             const int information,
                                                             const int value) {
    send_iiii(INFO_CHARACTERISTIC_WITH_VALUE,
              device_index,
              characteristic_index,
              information,
              value);
}


void Transceiver::send_feature_with_value(const int device_index,
                                          const int characteristic_index,
                                          const int feature_index,
                                          const float value) {
    send_iiif(INFO_FEATURE_WITH_VALUE,
              device_index,
              characteristic_index,
              feature_index,
              value);
}

void Transceiver::send_feature_with_value(const int device_index,
                                          const int characteristic_index,
                                          string &feature_name,
                                          const float value) {
    send_iisf(INFO_FEATURE_WITH_VALUE,
              device_index,
              characteristic_index,
              feature_name,
              value);
}

void Transceiver::send_watchdog(const int interval_in_milliseconds) {
    send_i(INFO_WATCHDOG,
           interval_in_milliseconds);
}
