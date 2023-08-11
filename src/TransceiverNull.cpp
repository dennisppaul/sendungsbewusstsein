#include "Transceiver.h"

#ifndef SB_HAS_OSC

#include <iostream>

using namespace std;

void Transceiver::osc_thread(const char *address,
                             int port_transmit,
                             int port_receive,
                             bool use_muilticast) {
}


void Transceiver::callback_receive(std::string typetag, std::vector<std::any> message) {}

Transceiver::Transceiver(const char *address,
                         int port_transmit,
                         int port_receive,
                         bool use_muilticast) {}

void Transceiver::initialize_transmit_socket(const char *address, int port_transmit) {}

void Transceiver::send_i(const int message, const int a) {}

void Transceiver::send_ii(const int message, const int a, const int b) {}

void Transceiver::send_iii(const int message, const int a, int b, const int c) {}

void Transceiver::send_iiii(const int message, const int a, int b, const int c, const int d) {}

void Transceiver::send_iiif(const int message, const int a, int b, const int c, const float d) {}

void Transceiver::send_iisf(const int message, const int a, int b, string &c, const float d) {}

void Transceiver::send_is(const int message, const int a, string &b) {}

void Transceiver::send_iis(const int message, const int a, const int b, string &c) {}

void Transceiver::send_iiis(const int message, const int a, const int b, const int c, string &d) {}

void Transceiver::send_sii(const int message, string &a, const int b, const int c) {}

#endif