#pragma once

// TODO convert this to be usable in processing/java

//#define DEBUG_INDOOR_BIKE_DATA_FEATURES
//#define DEBUG_CYCLING_POWER_MEASUREMENT_FEATURES
//#define DEBUG_HEART_RATE_MEASUREMENT_FEATURES

struct CommandItem {
    const int  cmd;
    const char *name;
    const char *typetag;
    const char *typetag_alt;
};

static const int CMD_SCAN_FOR_DEVICES                = 0;
static const int CMD_CONNECT_DEVICE                  = 1;
static const int CMD_DISCONNECT_DEVICE               = 2;
static const int CMD_SUBSCRIBE_TO_CHARACTERISTIC     = 3;
static const int CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC = 4;
static const int CMD_GET_DEVICE_NAME                 = 5;
static const int CMD_GET_CHARACTERISTIC_NAME         = 6;
static const int CMD_GET_FEATURE_NAME                = 7;
static const int CMD_GET_FEATURE_VALUE               = 8;
static const int CMD_SET_FEATURE_VALUE               = 9;
static const int NUM_COMMANDS                        = 10;

static const CommandItem COMMAND_MAP[NUM_COMMANDS] = {
        {CMD_SCAN_FOR_DEVICES,                "scan_for_devices",                "ii",    nullptr},
        {CMD_CONNECT_DEVICE,                  "connect_device",                  "ii",    "is"},
        {CMD_DISCONNECT_DEVICE,               "disconnect_device",               "ii",    nullptr},
        {CMD_SUBSCRIBE_TO_CHARACTERISTIC,     "subscribe_to_characteristic",     "iii",   nullptr},
        {CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC, "unsubscribe_from_characteristic", "iii",   nullptr},
        {CMD_GET_DEVICE_NAME,                 "get_device_name",                 "ii",    nullptr},
        {CMD_GET_CHARACTERISTIC_NAME,         "get_characteristic_name",         "iii",   nullptr},
        {CMD_GET_FEATURE_NAME,                "get_feature_name",                "iiii",  nullptr},
        {CMD_GET_FEATURE_VALUE,               "get_feature_value",               "iiii",  "iiis"},
        {CMD_SET_FEATURE_VALUE,               "set_feature_value",               "iiiif", "iiisf"}
};

/* command values */

static const int ALL_DEVICES = -1;

struct InformationItem {
    const int  cmd;
    const char *name;
    const char *typetag;
};

static const int INFO_ERROR                     = -1;
static const int INFO_DEVICE                    = 0;
static const int INFO_DEVICE_WITH_VALUE         = 1;
static const int INFO_CHARACTERISTIC            = 2;
static const int INFO_CHARACTERISTIC_WITH_VALUE = 3;
static const int INFO_FEATURE_WITH_VALUE        = 4;
static const int INFO_WATCHDOG                  = 5;
static const int NUM_INFORMATION                = 6;

static const InformationItem INFORMATION_MAP[NUM_INFORMATION] = {
        {INFO_DEVICE,                    "send_device_information",                    "iii"},
        {INFO_DEVICE_WITH_VALUE,         "send_device_information_with_value",         "iiii"},
        {INFO_CHARACTERISTIC,            "send_characteristic_information",            "iiii"},
        {INFO_CHARACTERISTIC_WITH_VALUE, "send_characteristic_information_with_value", "iiiii"},
        {INFO_FEATURE_WITH_VALUE,        "send_feature_with_value",                    "iiisf"},
        {INFO_WATCHDOG,                  "send_watchdog",                              "ii"}
};

/* information values */

static const int ERROR                               = -1;
static const int CONNECTED_DEVICE                    = 0;
static const int DISCONNECT_DEVICE                   = 1;
static const int NUMBER_OF_SUPPORTED_CHARACTERISTICS = 2;
static const int SUBSCRIBED                          = 3;
static const int UNSUBSCRIBED                        = 4;
