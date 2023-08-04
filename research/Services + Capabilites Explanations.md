# Services + Capabilites Explanations

explanation of services + capabilites for a collection of devices:

## Wahoo KICKR ####

| SERVICE + CHARACTERISTIC               | CAPAB... | DESCRIPTION                         |
|----------------------------------------|----------|-------------------------------------|
| 0000180a-0000-1000-8000-00805f9b34fb   | -        | Device Information                  |
| - 00002a29-0000-1000-8000-00805f9b34fb | R        | Manufacturer Name                   |
| - 00002a25-0000-1000-8000-00805f9b34fb | R        | Serial Number                       |
| - 00002a27-0000-1000-8000-00805f9b34fb | R        | Hardware Revision                   |
| - 00002a26-0000-1000-8000-00805f9b34fb | R        | Firmware Revision                   |
| a026ee01-0a7d-4ab3-97fa-f1500f9feb8b   | -        | ???                                 |
| - a026e002-0a7d-4ab3-97fa-f1500f9feb8b | WC, N    | ???                                 |
| - a026e004-0a7d-4ab3-97fa-f1500f9feb8b | N        | ???                                 |
| - a026e03b-0a7d-4ab3-97fa-f1500f9feb8b | WC, N    | ???                                 |
| a026ee03-0a7d-4ab3-97fa-f1500f9feb8b   | -        | ???                                 |
| - a026e00a-0a7d-4ab3-97fa-f1500f9feb8b | WC N     | ???                                 |
| 00001818-0000-1000-8000-00805f9b34fb   | -        | Cycling Power                       |
| - 00002a63-0000-1000-8000-00805f9b34fb | N        | Cycling Power Measurement           |
| - 00002a65-0000-1000-8000-00805f9b34fb | R        | Cycling Power Feature               |
| - 00002a5d-0000-1000-8000-00805f9b34fb | R        | Sensor Location                     |
| - a026e005-0a7d-4ab3-97fa-f1500f9feb8b | WR, I    | Wahoo Trainer                       |
| - 00002a66-0000-1000-8000-00805f9b34fb | WR, I    | Cycling Power Control Point         |
| 00001826-0000-1000-8000-00805f9b34fb   | -        | Fitness Machine                     | 
| - 00002acc-0000-1000-8000-00805f9b34fb | R        | Fitness Machine Feature             |
| - 00002ad3-0000-1000-8000-00805f9b34fb | R, N     | Training Status                     | 
| - 00002ad9-0000-1000-8000-00805f9b34fb | WR, I    | Fitness Machine Control Point       |
| - 00002ada-0000-1000-8000-00805f9b34fb | N        | Fitness Machine Status              |
| - 00002ad2-0000-1000-8000-00805f9b34fb | N        | Indoor Bike Data                    |
| - 00002ad6-0000-1000-8000-00805f9b34fb | R        | Supported resistance Level Range    |
| - 00002ad8-0000-1000-8000-00805f9b34fb | R        | Supported Power Range               |
| 0000181c-0000-1000-8000-00805f9b34fb   | -        | User Data                           |
| - 00002a98-0000-1000-8000-00805f9b34fb | R, WR    | Weight                              |
| a026ee0b-0a7d-4ab3-97fa-f1500f9feb8b   | -        | Wahoo Fitness Machine               |
| - a026e037-0a7d-4ab3-97fa-f1500f9feb8b | R, WC, N | Wahoo Fitness Machine Control Point |
| a026ee06-0a7d-4ab3-97fa-f1500f9feb8b   | -        | ???                                 |
| - a026e023-0a7d-4ab3-97fa-f1500f9feb8b | WC, N    | ???                                 |

## WHOOP #########

| SERVICE + CHARACTERISTIC               | CAPAB... | DESCRIPTION                         | DESC |
|----------------------------------------|----------|-------------------------------------|------|
| 61080001-8d6d-82b8-614a-1c8cb0f8dcc6   | -        | WHOOP custom                        |      |
| - 61080002-8d6d-82b8-614a-1c8cb0f8dcc6 | WR, WC   | ???                                 |      |
| - 61080003-8d6d-82b8-614a-1c8cb0f8dcc6 | N        | ???                                 | 2902 |
| - 61080004-8d6d-82b8-614a-1c8cb0f8dcc6 | N        | ???                                 | 2902 |
| - 61080005-8d6d-82b8-614a-1c8cb0f8dcc6 | N        | ???                                 | 2902 |
| - 61080007-8d6d-82b8-614a-1c8cb0f8dcc6 | N        | ???                                 | 2902 |
| 0000180d-0000-1000-8000-00805f9b34fb   | -        | Heart Rate                          |      |
| - 00002a37-0000-1000-8000-00805f9b34fb | N        | Heart Rate Measurement              | 2902 |
| 0000180a-0000-1000-8000-00805f9b34fb   | -        | Device Information                  |      |
| - 00002a29-0000-1000-8000-00805f9b34fb | R        | Manufacturer Name                   |      |
| 0000180f-0000-1000-8000-00805f9b34fb   | -        | Battery                             |      |
| - 00002a19-0000-1000-8000-00805f9b34fb | R, N     | Battery Level                       | 2902 |

---

## capability abbreviations

- R = read
- N = N
- WR = write_request
- WC = wrtie_command
- I = indicate
