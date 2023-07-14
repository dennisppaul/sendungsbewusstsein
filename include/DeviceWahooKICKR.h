#pragma once

#include "Device.h"

class DeviceWahooKICKR : public Device {
public:
    static constexpr const char *NAME = "Wahoo KICKR 3724";

    virtual ~DeviceWahooKICKR() = default;

    int ID() {
        return -1;
    }
};

/*
Connecting to Wahoo KICKR 3724 [4D250578-36AF-BE46-B680-3BF913009F67]
Successfully connected.
MTU: 20
Service: 0000180a-0000-1000-8000-00805f9b34fb          "This is the standard Device Information service."
  Characteristic: 00002a29-0000-1000-8000-00805f9b34fb "This characteristic within the Device Information service supports the read operation. The central device can request and read the value of this characteristic, which provides device information."
    Capabilities: read
  Characteristic: 00002a25-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: 00002a27-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: 00002a26-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
Service: a026ee01-0a7d-4ab3-97fa-f1500f9feb8b
  Characteristic: a026e002-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: write_command notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: a026e004-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: a026e03b-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: write_command notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
Service: a026ee03-0a7d-4ab3-97fa-f1500f9feb8b          ""
  Characteristic: a026e00a-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: write_command notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
Service: 00001818-0000-1000-8000-00805f9b34fb          ""
  Characteristic: 00002a63-0000-1000-8000-00805f9b34fb ""
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002a65-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: 00002a5d-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: a026e005-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: write_request indicate
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002a66-0000-1000-8000-00805f9b34fb ""
    Capabilities: write_request indicate
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
Service: 00001826-0000-1000-8000-00805f9b34fb          ""
  Characteristic: 00002acc-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: 00002ad3-0000-1000-8000-00805f9b34fb ""
    Capabilities: read notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002ad9-0000-1000-8000-00805f9b34fb ""
    Capabilities: write_request indicate
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002ada-0000-1000-8000-00805f9b34fb ""
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002ad2-0000-1000-8000-00805f9b34fb ""
    Capabilities: notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
  Characteristic: 00002ad6-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
  Characteristic: 00002ad8-0000-1000-8000-00805f9b34fb ""
    Capabilities: read
Service: 0000181c-0000-1000-8000-00805f9b34fb          ""
  Characteristic: 00002a98-0000-1000-8000-00805f9b34fb ""
    Capabilities: read write_request
Service: a026ee0b-0a7d-4ab3-97fa-f1500f9feb8b          ""
  Characteristic: a026e037-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: read write_command notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
Service: a026ee06-0a7d-4ab3-97fa-f1500f9feb8b          ""
  Characteristic: a026e023-0a7d-4ab3-97fa-f1500f9feb8b ""
    Capabilities: write_command notify
    Descriptor: 00002902-0000-1000-8000-00805f9b34fb
 */

// *interpretations* of services and characteristics based on code from GitHub repositories

/*
### Wahoo KICKR 3724 [4D250578-36AF-BE46-B680-3BF913009F67]

- Service: `0000180a-0000-1000-8000-00805f9b34fb` > "deviceInformation"
    - Characteristic: `00002a29-0000-1000-8000-00805f9b34fb` > "manufacturerNameString" [read]
    - Characteristic: `00002a25-0000-1000-8000-00805f9b34fb` > "serial_number_string" [read]
    - Characteristic: `00002a27-0000-1000-8000-00805f9b34fb` > "hardware_revision_string" [read]
    - Characteristic: `00002a26-0000-1000-8000-00805f9b34fb` > "firmwareRevisionString" [read]
- Service: `a026ee01-0a7d-4ab3-97fa-f1500f9feb8b`
- Service: `a026ee03-0a7d-4ab3-97fa-f1500f9feb8b`
- Service: `00001818-0000-1000-8000-00805f9b34fb` > "cyclingPower"
    - Characteristic: `00002a63-0000-1000-8000-00805f9b34fb` > "cyclingPowerMeasurement" [notify]
    - Characteristic: `00002a65-0000-1000-8000-00805f9b34fb` > "cyclingPowerFeature" [read]
    - Characteristic: `00002a5d-0000-1000-8000-00805f9b34fb` > "sensorLocation" [read]
    - Characteristic: `a026e005-0a7d-4ab3-97fa-f1500f9feb8b` > "wahooTrainer" [write_request indicate]
    - Characteristic: `00002a66-0000-1000-8000-00805f9b34fb` > "cyclingPowerControlPoint" [write_request indicate]
- Service: `00001826-0000-1000-8000-00805f9b34fb` > "fitnessMachine"
    - Characteristic: `00002acc-0000-1000-8000-00805f9b34fb` > "fitnessMachineFeature" [read]
    - Characteristic: `00002ad3-0000-1000-8000-00805f9b34fb` > "training_status" [read notify]
    - Characteristic: `00002ad9-0000-1000-8000-00805f9b34fb` > "fitnessMachineControlPoint" [write_request indicate]
    - Characteristic: `00002ada-0000-1000-8000-00805f9b34fb` > "fitnessMachineStatus" [notify]
    - Characteristic: `00002ad2-0000-1000-8000-00805f9b34fb` > "indoorBikeData" [notify]
    - Characteristic: `00002ad6-0000-1000-8000-00805f9b34fb` > "supportedResistanceLevelRange" [read]
    - Characteristic: `00002ad8-0000-1000-8000-00805f9b34fb` > "supportedPowerRange" [read]
- Service: `0000181c-0000-1000-8000-00805f9b34fb` > "user data"
    - Characteristic: `00002a98-0000-1000-8000-00805f9b34fb` > "weight" [read write_request]
- Service: `a026ee0b-0a7d-4ab3-97fa-f1500f9feb8b` > "wahooFitnessMachine"
    - Characteristic: `a026e037-0a7d-4ab3-97fa-f1500f9feb8b` > "wahooFitnessMachineControlPoint" [read write_command notify]
- Service: `a026ee06-0a7d-4ab3-97fa-f1500f9feb8b`
 */

// *interpretations* of services and characteristics based on ChatGPT

/*
Based on the provided attributes for a Wahoo KICKR device, here is an explanation of their meanings:

Service: 0000180a-0000-1000-8000-00805f9b34fb
This is the standard Device Information service.
Characteristic: 00002a29-0000-1000-8000-00805f9b34fb
Capabilities: read
This characteristic within the Device Information service supports the read operation. The central device can request and read the value of this characteristic, which provides device information.
Characteristic: 00002a25-0000-1000-8000-00805f9b34fb
Capabilities: read
This characteristic within the Device Information service supports the read operation. It allows the central device to read certain information about the device.
Characteristic: 00002a27-0000-1000-8000-00805f9b34fb
Capabilities: read
This characteristic within the Device Information service supports the read operation. The central device can read the value of this characteristic to obtain specific device information.
Characteristic: 00002a26-0000-1000-8000-00805f9b34fb
Capabilities: read
This characteristic within the Device Information service supports the read operation. The central device can read the value of this characteristic to get information about the firmware version of the device.
Service: a026ee01-0a7d-4ab3-97fa-f1500f9feb8b
This is a custom service provided by the Wahoo KICKR device.
Characteristic: a026e002-0a7d-4ab3-97fa-f1500f9feb8b
Capabilities: write_command notify
This characteristic supports both the write_command and notify operations. The central device can send commands or data using write commands, and the KICKR device can send notifications to the central device.
Characteristic: a026e004-0a7d-4ab3-97fa-f1500f9feb8b
Capabilities: notify
This characteristic supports the notify operation. The KICKR device can send notifications to the central device.
Characteristic: a026e03b-0a7d-4ab3-97fa-f1500f9feb8b
Capabilities: write_command notify
This characteristic supports both the write_command and notify operations. The central device can send commands or data using write commands, and the KICKR device can send notifications to the central device.
Service: 00001818-0000-1000-8000-00805f9b34fb
This is the standard Cycling Power service.
Characteristic: 00002a63-0000-1000-8000-00805f9b34fb
Capabilities: notify
This characteristic within the Cycling Power service supports the notify operation. The KICKR device can send power notifications to the central device.
Characteristic: 00002a65-0000-1000-8000-00805f9b34fb
Capabilities: read
This characteristic within the Cycling Power service supports the read operation. The central device can request and read the value of this characteristic, which provides specific power-related information.
These attributes define the services and characteristics offered by the Wahoo KICKR device, along with their corresponding capabilities such as read, write_command, and notify.
 */