#pragma once

/*
 * see [GATT Specification Supplement (v10)](https://www.bluetooth.com/specifications/specs/gatt-specification-supplement/)
 * and [Fitness Machine Service (v1.0)](https://www.bluetooth.com/specifications/specs/fitness-machine-service-1-0/)
 */
/* GATT Specification Supplement */
static constexpr const char *SERVICE_WHOOP_CUSTOM                                       = "61080001-8d6d-82b8-614a-1c8cb0f8dcc6";
static constexpr const char *CHARACTERISTIC_01_WRWC                                     = "61080002-8d6d-82b8-614a-1c8cb0f8dcc6";
static constexpr const char *CHARACTERISTIC_02_N                                        = "61080003-8d6d-82b8-614a-1c8cb0f8dcc6";
static constexpr const char *CHARACTERISTIC_03_N                                        = "61080004-8d6d-82b8-614a-1c8cb0f8dcc6";
static constexpr const char *CHARACTERISTIC_04_N                                        = "61080005-8d6d-82b8-614a-1c8cb0f8dcc6";
static constexpr const char *CHARACTERISTIC_05_N                                        = "61080007-8d6d-82b8-614a-1c8cb0f8dcc6";
/* Heart Rate */
static constexpr const char *SERVICE_HEART_RATE                                         = "0000180d-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_HEART_RATE_MEASUREMENT_N                    = "00002a37-0000-1000-8000-00805f9b34fb";
static constexpr const char *SERVICE_DEVICE_INFORMATION_UUID                            = "0000180a-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_MANUFACTURER_NAME_R                         = "00002a29-0000-1000-8000-00805f9b34fb";
/* Battery */
static constexpr const char *SERVICE_BATTERY                                            = "0000180f-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_BATTERY_LEVEL_RN                            = "00002a19-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_DESCRIPTOR                                  = "00002902-0000-1000-8000-00805f9b34fb";
/* Cycling Power */
static constexpr const char *SERVICE_CYCLING_POWER                                      = "00001818-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N                 = "00002a63-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_CYCLING_POWER_FEATURE_R                     = "00002a65-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_SENSOR_LOCATION_R                           = "00002a5d-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_WAHOO_TRAINER_WRI                           = "a026e005-0a7d-4ab3-97fa-f1500f9feb8b";
static constexpr const char *CHARACTERISTIC_CYCLING_POWER_CONTROL_POINT_WRI             = "00002a66-0000-1000-8000-00805f9b34fb";
/* Cycling Speed and Cadence (CSC) ( see GATT Specification Supplement p66ff ) */
static constexpr const char *SERVICE_CYCLING_SPEED_AND_CADENCE                          = "00001816-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_CYCLING_SPEED_AND_CADENCE_MEASUREMENT_N     = "00002a5b-0000-1000-8000-00805f9b34fb"; // "Provides speed and cadence data from a cycling sensor. It typically includes information like wheel revolutions, last wheel event time, crank revolutions, and last crank event time."
static constexpr const char *CHARACTERISTIC_CYCLING_SPEED_AND_CADENCE_FEATURE_R         = "00002a5c-0000-1000-8000-00805f9b34fb"; // "Describes the supported features of the sensor (e.g., whether it supports both speed and cadence measurement or only one of these)."
static constexpr const char *CHARACTERISTIC_CYCLING_SPEED_AND_CADENCE_SENSOR_LOCATION_R = "00002a5d-0000-1000-8000-00805f9b34fb"; // "Indicates the location of the sensor (e.g., rear wheel, front wheel, left pedal, right pedal, etc.)."
static constexpr const char *CHARACTERISTIC_CYCLING_SPEED_AND_CADENCE_CONTROL_POINT_WRI = "00002a55-0000-1000-8000-00805f9b34fb"; // ""SC Control Point" belongs to the Cycling Speed and Cadence (CSC) service. This characteristic is typically used to enable and control specific features of a CSC sensor, such as calibration or other specific functions that the sensor may support. It provides a way for a client device to send control-related commands to the CSC sensor."
/* Fitness Machine Service */
static constexpr const char *SERVICE_FITNESS_MACHINE                                    = "00001826-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_FITNESS_MACHINE_FEATURE_R                   = "00002acc-0000-1000-8000-00805f9b34fb"; // "Fitness Machine Service", p19
static constexpr const char *CHARACTERISTIC_TRAINING_STATUS_RN                          = "00002ad3-0000-1000-8000-00805f9b34fb"; // "Fitness Machine Service", p46
static constexpr const char *CHARACTERISTIC_FITNESS_MACHINE_CONTROLL_POINT_WRI          = "00002ad9-0000-1000-8000-00805f9b34fb"; // "Fitness Machine Service", p50
static constexpr const char *CHARACTERISTIC_FITNESS_MACHINE_STATUS_N                    = "00002ada-0000-1000-8000-00805f9b34fb"; // "Fitness Machine Service", p66
static constexpr const char *CHARACTERISTIC_INDOOR_BIKE_DATA_N                          = "00002ad2-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_SUPPORTED_RESISTENCE_LEVEL_RANGE_R          = "00002ad6-0000-1000-8000-00805f9b34fb";
static constexpr const char *CHARACTERISTIC_SUPPORTED_POWER_RANGE_R                     = "00002ad8-0000-1000-8000-00805f9b34fb";

/* ... more detailed descriptions of characteristics can be found in respective characteristic implementations */