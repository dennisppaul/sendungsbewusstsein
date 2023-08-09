#pragma once

#include "Device.h"

void scan_for_devices(int timeout_ms,
                      bool verbose,
                      bool ignore_devices_without_name = true);

int get_number_of_available_devices();

Device *get_device(int connected_device_index);

int connect_device(int peripheral_index);

int connect_device(std::string &name_or_UUID);