#pragma once

//#define DEBUG_INDOOR_BIKE_DATA_FEATURES
//#define DEBUG_CYCLING_POWER_MEASUREMENT_FEATURES
//#define DEBUG_HEART_RATE_MEASUREMENT_FEATURES

/*
### commands ( from client to server )

- `scan_for_devices`
- `connect_device`
- `disconnect_device`
- `subscribe_to_characteristic`
- `unsubscribe_from_characteristic`
- `get_characteristic_name`
- `get_feature_name`
- `get_feature_value`
- `set_feature_value`
 */

/*
### information ( from server to client )

- `send_device_information`
- `send_device_information_with_value`
- `send_characteristic_information`
- `send_characteristic_feature_with_value`

 */

static const int CMD_SCAN_FOR_DEVICES                = 0;
static const int CMD_CONNECT_DEVICE                  = 1;
static const int CMD_DISCONNECT_DEVICE               = 2;
static const int CMD_SUBSCRIBE_TO_CHARACTERISTIC     = 3;
static const int CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC = 4;
static const int CMD_GET_CHARACTERISTIC_NAME         = 5;
static const int CMD_GET_FEATURE_NAME                = 6;
static const int CMD_GET_FEATURE_VALUE               = 7;
static const int CMD_SET_FEATURE_VALUE               = 8;

static const int            INFO_ERROR             = -1;
static const int            INFO_DEVICE            = 0;
static const int            INFO_DEVICE_WITH_VALUE = 1;
static const int            INFO_CHARACTERISTIC    = 2;
static const int            INFO_FEATURE           = 3;
static constexpr const char *INFO_WATCHDOG         = "watchdog"; // TODO maybe change this to `ìnt`, too?

static const int EROOR                               = -1;
static const int CONNECTED_DEVICE                    = 0;
static const int DISCONNECT_DEVICE                   = 1;
static const int NUMBER_OF_SUPPORTED_CHARACTERISTICS = 2;
static const int SUBSCRIBED                          = 3;
static const int UNSUBSCRIBED                        = 4;
