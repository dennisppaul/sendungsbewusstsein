#pragma once

#include <iostream>
#include <thread>
#include <any>
#include <utility>
#include <vector>
#include <string>

#include "Sendungsbewusstsein.h"
#include "Console.h"
#include "StringUtils.h"

#define DEBUG_OSC
#define DEFAULT_OSC_TRANSMIT_ADDRESS        "127.0.0.1"
#define DEFAULT_OSC_TRANSMIT_PORT           7000
#define DEFAULT_OSC_RECEIVE_PORT            7001
#define DEFAULT_USE_UDP_MULTICAST           false

using namespace std;

static const constexpr char *SB_OSC_ADDRESS_PATTERN = "/sendungsbewusstsein/";

class Transceiver {
public:
    static Transceiver *init(const char *address, int port_transmit, int port_receive, bool use_muilticast) {
        if (fInstance == nullptr) {
            console << "@OSC initializing OSC with user values" << endl;
            fInstance = new Transceiver(address, port_transmit, port_receive, use_muilticast);
        }
        return fInstance;
    }

    static Transceiver *instance() {
        if (fInstance == nullptr) {
            console << "@OSC starting OSC with default values" << endl;
            fInstance = new Transceiver(DEFAULT_OSC_TRANSMIT_ADDRESS,
                                        DEFAULT_OSC_TRANSMIT_PORT,
                                        DEFAULT_OSC_RECEIVE_PORT,
                                        DEFAULT_USE_UDP_MULTICAST);
        }
        return fInstance;
    }

    Transceiver(const Transceiver &) = delete;

    Transceiver &operator=(const Transceiver &) = delete;

    void finalize() {
        mOSCThread->detach();
        delete mOSCThread;
//        delete mTransmitSocket;
    }

    void scan_for_devices(int duration_in_milliseconds, int number_of_devices);

    void connect_device(int available_device_index,
                        int device_index,
                        int number_of_characteristics);

    void connect_device(string &name_or_UUID,
                        int device_index,
                        int number_of_characteristics);

    void disconnect_device(int device_index);

    void subscribe_to_characteristic(int device_index,
                                     int characteristic_index,
                                     int number_of_features);

    void unsubscribe_from_characteristic(int device_index,
                                         int characteristic_index);

    void get_device_name(int device_index,
                         string &device_name);

    void get_characteristic_name(int device_index,
                                 int characteristic_index,
                                 string &characteristic_name);

    void get_feature_name(int device_index,
                          int characteristic_index,
                          int feature_index,
                          string &feature_name);

    void get_feature_value(int device_index,
                           int characteristic_index,
                           int feature_index,
                           float value);

    void get_feature_value(int device_index,
                           int characteristic_index,
                           string &feature_name,
                           float value);

    void set_feature_value(int device_index,
                           int characteristic_index,
                           int feature_index,
                           float value);

    void set_feature_value(int device_index,
                           int characteristic_index,
                           string &feature_name,
                           float value);

    void send_device_information(int device_index,
                                 int information);

    void send_device_information_with_value(int device_index,
                                            int information,
                                            int value);

    void send_characteristic_information(int device_index,
                                         int characteristic_index,
                                         int information);

    void send_characteristic_information_with_value(int device_index,
                                                    int characteristic_index,
                                                    int information,
                                                    int value);

    void send_feature_with_value(int device_index,
                                 int characteristic_index,
                                 int feature_index,
                                 float value);

    void send_watchdog(int interval_in_milliseconds);

    void callback_receive(std::string typetag, std::vector<std::any> message);

private:
    typedef void (*CallbackType3_STRING_STRING_ANY)(std::string, std::vector<std::any>);

    static Transceiver              *fInstance;
    static const uint16_t           OSC_TRANSMIT_OUTPUT_BUFFER_SIZE = 1024;
    thread                          *mOSCThread                     = nullptr;
    CallbackType3_STRING_STRING_ANY pCallback                       = nullptr;

    Transceiver(const char *address,
                int port_transmit,
                int port_receive,
                bool use_muilticast);

    void osc_thread(const char *address,
                    int port_transmit,
                    int port_receive,
                    bool use_muilticast);

    void initialize_transmit_socket(const char *address, int port_transmit);

    void send_i(const int message, const int a);

    void send_ii(const int message, const int a, const int b);

    void send_iii(const int message, const int a, int b, const int c);

    void send_iiii(const int message, const int a, int b, const int c, const int d);

    void send_iiif(const int message, const int a, int b, const int c, const float d);

    void send_iisf(const int message, const int a, int b, string &c, const float d);

    void send_is(const int message, const int a, string &b);

    void send_iis(const int message, const int a, const int b, string &c);

    void send_iiis(const int message, const int a, const int b, const int c, string &d);

    void send_sii(const int message, string &a, const int b, const int c);

public:
    void register_receive_callback(CallbackType3_STRING_STRING_ANY callback) {
        pCallback = callback;
    }
};
