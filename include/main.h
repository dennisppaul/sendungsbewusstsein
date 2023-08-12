#pragma once

#include "Sendungsbewusstsein.h"
#include "Device.h"

void scan_for_devices(int timeout_ms,
                      bool verbose,
                      bool ignore_devices_without_name = true);

int get_number_of_available_devices();

const shared_ptr<Device> &get_device(int connected_device_index);

void reset_connected_devices();

int get_number_of_connected_devices();

int connect_device(int peripheral_index);

int connect_device(std::string &name_or_UUID);

int disconnect_device(int connected_device_index);
