#pragma once

//#define DEBUG_INDOOR_BIKE_DATA_FEATURES
//#define DEBUG_CYCLING_POWER_MEASUREMENT_FEATURES
//#define DEBUG_HEART_RATE_MEASUREMENT_FEATURES

/*
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

static const constexpr char *CMD_SCAN_FOR_DEVICES                = "scan_for_devices";
static const constexpr char *CMD_CONNECT_DEVICE                  = "connect_device";
static const constexpr char *CMD_DISCONNECT_DEVICE               = "disconnect_device";
static const constexpr char *CMD_SUBSCRIBE_TO_CHARACTERISTIC     = "subscribe_to_characteristic";
static const constexpr char *CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC = "unsubscribe_from_characteristic";
static const constexpr char *CMD_GET_CHARACTERISTIC_NAME         = "get_characteristic_name";
static const constexpr char *CMD_GET_FEATURE_NAME                = "get_feature_name";
static const constexpr char *CMD_GET_FEATURE_VALUE               = "get_feature_value";
static const constexpr char *CMD_SET_FEATURE_VALUE               = "set_feature_value";
static const constexpr char *CMD_RESPONSE_SUBSCRIBED             = "subscribed";
static const constexpr char *CMD_RESPONSE_UNSUBSCRIBED           = "unsubscribed";
