# Services + Capabilites Explanations

explanation of services + capabilites for a collection of devices:

## Wahoo KICKR ####

| SERVICE + CHARACTERISTIC               | CAPAB... | DESCRIPTION                         |
|----------------------------------------|----------|-------------------------------------|
| 0000180a-0000-1000-8000-00805f9b34fb   | -        | device information                  |
| - 00002a29-0000-1000-8000-00805f9b34fb | R        | manufacturer name                   |
| - 00002a25-0000-1000-8000-00805f9b34fb | R        | serial number                       |
| - 00002a27-0000-1000-8000-00805f9b34fb | R        | hardware revision                   |
| - 00002a26-0000-1000-8000-00805f9b34fb | R        | firmware revision                   |
| a026ee01-0a7d-4ab3-97fa-f1500f9feb8b   | -        | ???                                 |
| - a026e002-0a7d-4ab3-97fa-f1500f9feb8b | WC, N    | ???                                 |
| - a026e004-0a7d-4ab3-97fa-f1500f9feb8b | N        | ???                                 |
| - a026e03b-0a7d-4ab3-97fa-f1500f9feb8b | WC, N    | ???                                 |
| a026ee03-0a7d-4ab3-97fa-f1500f9feb8b   | -        | ???                                 |
| - a026e00a-0a7d-4ab3-97fa-f1500f9feb8b | WC N     | ???                                 |
| 00001818-0000-1000-8000-00805f9b34fb   | -        | cycling power                       |
| - 00002a63-0000-1000-8000-00805f9b34fb | N        | cycling power measurement           |
| - 00002a65-0000-1000-8000-00805f9b34fb | R        | cycling power feature               |
| - 00002a5d-0000-1000-8000-00805f9b34fb | R        | sensor location                     |
| - a026e005-0a7d-4ab3-97fa-f1500f9feb8b | WR, I    | wahoo trainer                       |
| - 00002a66-0000-1000-8000-00805f9b34fb | WR, I    | cycling power control point         |
| 00001826-0000-1000-8000-00805f9b34fb   | -        | fitness machine                     | 
| - 00002acc-0000-1000-8000-00805f9b34fb | R        | fitness machine feature             |
| - 00002ad3-0000-1000-8000-00805f9b34fb | R, N     | training status                     | 
| - 00002ad9-0000-1000-8000-00805f9b34fb | WR, I    | fitness machine control point       |
| - 00002ada-0000-1000-8000-00805f9b34fb | N        | fitness machine status              |
| - 00002ad2-0000-1000-8000-00805f9b34fb | N        | indoor bike data                    |
| - 00002ad6-0000-1000-8000-00805f9b34fb | R        | supported resistance level range    |
| - 00002ad8-0000-1000-8000-00805f9b34fb | R        | supported power range               |
| 0000181c-0000-1000-8000-00805f9b34fb   | -        | user data                           |
| - 00002a98-0000-1000-8000-00805f9b34fb | R, WR    | weight                              |
| a026ee0b-0a7d-4ab3-97fa-f1500f9feb8b   | -        | wahoo fitness machine               |
| - a026e037-0a7d-4ab3-97fa-f1500f9feb8b | R, WC, N | wahoo fitness machine control point |
| a026ee06-0a7d-4ab3-97fa-f1500f9feb8b   | -        | ???                                 |
| - a026e023-0a7d-4ab3-97fa-f1500f9feb8b | WC, N    | ???                                 |

## WHOOP #########

| SERVICE + CHARACTERISTIC               | CAPAB... | DESCRIPTION                         |
|----------------------------------------|----------|-------------------------------------|
| 61080001-8d6d-82b8-614a-1c8cb0f8dcc6   | -        | WHOOP custom                        |
| - 61080002-8d6d-82b8-614a-1c8cb0f8dcc6 | WR, WC   | ???                                 |
| - 61080003-8d6d-82b8-614a-1c8cb0f8dcc6 | N        | ???                                 |
| - 61080004-8d6d-82b8-614a-1c8cb0f8dcc6 | N        | ???                                 |
| - 61080005-8d6d-82b8-614a-1c8cb0f8dcc6 | N        | ???                                 |
| - 61080007-8d6d-82b8-614a-1c8cb0f8dcc6 | N        | ???                                 |
| 0000180d-0000-1000-8000-00805f9b34fb   | -        | heart rate                          |
| - 00002a37-0000-1000-8000-00805f9b34fb | N        | heart rate measurement              |
| 0000180a-0000-1000-8000-00805f9b34fb   | -        | device information                  |
| - 00002a29-0000-1000-8000-00805f9b34fb | R        | manufacturer name                   |
| 0000180f-0000-1000-8000-00805f9b34fb   | -        | battery                             |
| - 00002a19-0000-1000-8000-00805f9b34fb | R, N     | battery level                       |

---

## capability abbreviations

- R = read
- N = N
- WR = write_request
- WC = wrtie_command
- I = indicate
