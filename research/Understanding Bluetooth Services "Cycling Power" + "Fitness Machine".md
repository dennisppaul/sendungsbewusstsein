# Understanding Bluetooth Services "Cycling Power" + "Fitness Machine"

most information can be found in "GATT Specification Supplement" written by the "Bluetooth Specification Automation Task Force (SATF)" and "Fitness Machine Service 1.0" by the "Sports and Fitness Working Group".

the principle seems to be always the same: the characteristics "Cycling Power Measurement" and "Indoor Bike Data" can be subscribed to and deliver continous data. in both cases the first 2 bytes contain a series of flags that define the amount of paramaters i.e the amount of bytes to follow. the format of the parameters ( e.g uint8, uint16, int16 … ) is specified per paramter. 

for wahoo 

## Characteristic "Cycling Power Feature" from service "Cycling Power"

see "GATT Specification Supplement 10: 3.64 Cycling Power Feature" (p73) for details.

the *Cycling Power Feature* field is comprised of 32 bits ( or 4 octets ( i.e bytes ) ).

### Table: Cycling Power Feature ( 3.64.1 Cycling Power Feature field, Table 3.111: Cycling Power Feature field )

| Bit | Definition                                                         |
|-----|--------------------------------------------------------------------|
| 0   | Pedal Power Balance Supported                                      |
| 1   | Accumulated Torque Supported                                       |
| 2   | Wheel Revolution Data Supported                                    |
| 3   | Crank Revolution Data Supported                                    |
| 4   | Extreme Magnitudes Supported                                       |
| 5   | Extreme Angles Supported                                           |
| 6   | Top and Bottom Dead Spot Angles Supported                          |
| 7   | Accumulated Energy Supported                                       |
| 8   | Offset Compensation Indicator Supported                            |
| 9   | Offset Compensation Supported                                      |
| 10  | Cycling Power Measurement Characteristic Content Masking Supported |
| 11  | Multiple Sensor Locations Supported                                |
| 12  | Crank Length Adjustment Supported                                  |
| 13  | Chain Length Adjustment Supported                                  |
| 14  | Chain Weight Adjustment Supported                                  |
| 15  | Span Length Adjustment Supported                                   |
| 16  | Sensor Measurement Context ( 0 = Force based, 1 = Torque based )   |
| 17  | Instantaneous Measurement Direction Supported                      |
| 18  | Factory Calibration Date Supported                                 |
| 19  | Enhanced Offset Compensation Procedure Supported                   |
| 20  | Distributed System Support ( first bit )                           |
| 21  | Distributed System Support ( second bit )                          |
| 22  | RFU                                                                |
| 23  | RFU                                                                |
| 24  | RFU                                                                |
| 25  | RFU                                                                |
| 26  | RFU                                                                |
| 27  | RFU                                                                |
| 28  | RFU                                                                |
| 29  | RFU                                                                |
| 30  | RFU                                                                |
| 31  | RFU                                                                |

### examples

@problem(the *Cycling Power Feature* field is supposed to be comprised of 4 bytes, not 2 bytes?!?)

```
+++ reading CHARACTERISTIC_CYCLING_POWER_FEATURE_R  : 01110000 01001000 ( 0x0e 0x12 )

01110000 // Accumulated Torque Supported, Wheel Revolution Data Supported, Crank Revolution Data Supported
01001000 // Offset Compensation Supported, Crank Length Adjustment Supported 
```

## Characteristic "Cycling Power Measurement" from service "Cycling Power"

see "GATT Specification Supplement 10: 3.65 Cycling Power Measurement" (p74) for details.

### Table: Parameters ( 3.65 Cycling Power Measurement, Table 3.112: Structure of the Cycling Power Measurement characteristic )

| Field                     | Data Type    | Size (in octets) | Description             |
|---------------------------|--------------|------------------|-------------------------|
| Flags                     | boolean[16]  | 2                |                         |
| Instantaneous Power       | sint16       | 2                | unit: watts             |
| Pedal Power Balance       | uint8        | 0 or 1           | unit: 1/2 percent       |
| Accumulated Torque        | uint16       | 0 or 2           | unit: 1/32 Newton meter |
| Wheel Revolution Data     | struct       | 0 or 6           |                         |
| Crank Revolution Data     | struct       | 0 or 4           |                         |
| Extreme Force Magnitudes  | struct       | 0 or 4           |                         |
| Extreme Torque Magnitudes | struct       | 0 or 4           |                         |
| Extreme Angles            | struct       | 0 or 3           |                         |
| Top Dead Spot Angle       | uint16       | 0 or 2           | unit: angle degree      |
| Bottom Dead Spot Angle    | uint16       | 0 or 2           | unit: angle degree      |
| Accumulated Energy        | uint16       | 0 or 2           | unit: kilojoule         |

### Table: Flags ( 3.65.1 Flags field, Table 3.113: Flags field )

| Bit   | Definition                                                     |
|-------|----------------------------------------------------------------|
| 0     | Pedal Power Balance Present                                    |
| 1     | Pedal Power Balance Reference ( 0 = Unknown, 1 = Left )        |
| 2     | Accumulated Torque Present                                     |
| 3     | Accumulated Torque Source ( 0 = Wheel based, 1 = Crank based ) |
| 4     | Wheel Revolution Data Present                                  |
| 5     | Crank Revolution Data Present                                  |
| 6     | Extreme Force Magnitudes Present                               |
| 7     | Extreme Torque Magnitudes Present                              |
| 8     | Extreme Angles Present                                         |
| 9     | Top Dead Spot Angle Present                                    |
| 10    | Bottom Dead Spot Angle Present                                 |
| 11    | Accumulated Energy Present                                     |
| 12    | Offset Compensation Indicator ( 0 = False, 1 = True )          |
| 13–15 | Reserved for Future Use (RFU)                                  |

### example

breaking down a single reading:

```
+++ reading CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N

> 34 00 84 00 a8 17 22 00 00 00 6b fc 02 00 f4 06 
```

as specified in the paramater table the first 2 bytes ( `0x34 0x00` ) contain the flags. according to the flag table the reading contains the following parameters:

```
00101100 00000000 ( 0x34 0x00 )

00101100 // Accumulated Torque, (Accumulated Torque Source=Wheel based), Wheel Revolution Data, Crank Revolution Data
00000000 // 
```

the remaining 14 bytes are distributed in order of appearence according to the parameter table which specifies the following length of each parameter in bytes:

```
2 :: Instantaneous Power ( sint16 )
2 :: Accumulated Torque ( unint16 )
6 :: Wheel Revolution Data ( Cumulative Wheel Revolutions + Last Wheel Event Time ) ( uint32 + uin16 )
4 :: Crank Revolution Data ( Cumulative Crank Revolutions + Last Crank Event Time ) ( uint16 +uint16 )
```

the bytes are distributes as follows:

```
34 00       :: flags
84 00       :: Instantaneous Power ( sint16 "Unit: org.bluetooth.unit.power.watt" ) // this seems to be always present?!?
a8 17       :: Accumulated Torque ( "Unit is 1/32 Newton meter" )
22 00 00 00 :: Wheel Revolution Data ( uint32, Cumulative Wheel Revolutions )
6b fc       :: Wheel Revolution Data ( uint16, Last Wheel Event Time )
02 00       :: Crank Revolution Data ( uint16, Cumulative Crank Revolutions )
f4 06       :: Crank Revolution Data ( uint16, Last Crank Event Time )
```

note, how "Wheel Revolution Data" + "Crank Revolution Data" are broken down into two values ( as seen in "3.65.2 Wheel Revolution Data field" and "3.65.3 Crank Revolution Data field" ).

a longer reading:

```
+++ reading CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_N

> 34 00 00 00 00 00 08 00 00 00 84 14 00 00 00 00 
...
> 34 00 00 00 00 00 08 00 00 00 84 14 00 00 00 00 
> 34 00 49 00 a6 02 0b 00 00 00 9e b4 00 00 00 00 
> 34 00 13 00 ea 02 0c 00 00 00 33 ba 00 00 00 00 
> 34 00 93 00 0c 05 0e 00 00 00 f5 c1 00 00 00 00 
> 34 00 bd 00 6d 09 11 00 00 00 28 cb 00 00 00 00 
> 34 00 b4 00 17 0d 14 00 00 00 7e d3 00 00 00 00 
> 34 00 9e 00 99 10 17 00 00 00 dd db 00 00 00 00 
> 34 00 87 00 82 13 1a 00 00 00 37 e4 00 00 00 00 
> 34 00 42 00 c2 14 1c 00 00 00 3e ea 02 00 f4 06 
> 34 00 2a 00 6c 15 1f 00 00 00 7e f3 02 00 f4 06 
> 34 00 84 00 a8 17 22 00 00 00 6b fc 02 00 f4 06 
> 34 00 80 01 1b 1d 25 00 00 00 0a 04 05 00 3b 11 
> 34 00 eb 01 95 25 28 00 00 00 3a 0b 07 00 fd 17 
> 34 00 54 01 e1 2c 2b 00 00 00 50 12 08 00 30 1b 
> 34 00 2c 00 d3 2f 2e 00 00 00 79 1a 08 00 30 1b 
> 34 00 00 00 d3 2f 31 00 00 00 0c 23 08 00 30 1b 
> 34 00 00 00 d3 2f 34 00 00 00 f6 2b 0a 00 7e 27 
> 34 00 00 00 d3 2f 36 00 00 00 1a 32 0a 00 7e 27 
> 34 00 00 00 d3 2f 39 00 00 00 a4 3b 0a 00 7e 27 
> 34 00 00 00 d3 2f 3b 00 00 00 3e 42 0a 00 7e 27 
> 34 00 00 00 d3 2f 3e 00 00 00 8b 4c 0a 00 7e 27 
> 34 00 00 00 d3 2f 40 00 00 00 b6 53 0a 00 7e 27 
> 34 00 00 00 d3 2f 42 00 00 00 26 5b 0a 00 7e 27 
> 34 00 00 00 d3 2f 44 00 00 00 e9 62 0a 00 7e 27 
> 34 00 00 00 d3 2f 46 00 00 00 19 6b 0a 00 7e 27 
> 34 00 00 00 d3 2f 48 00 00 00 cb 73 0a 00 7e 27 
> 34 00 00 00 d3 2f 49 00 00 00 5c 78 0a 00 7e 27 
> 34 00 00 00 d3 2f 4b 00 00 00 03 82 0a 00 7e 27 
> 34 00 00 00 d3 2f 4d 00 00 00 7f 8c 0a 00 7e 27 
> 34 00 00 00 d3 2f 4e 00 00 00 20 92 0a 00 7e 27 
> 34 00 00 00 d3 2f 4f 00 00 00 12 98 0a 00 7e 27 
> 34 00 00 00 d3 2f 50 00 00 00 62 9e 0a 00 7e 27 
> 34 00 00 00 d3 2f 52 00 00 00 7b ac 0a 00 7e 27 
> 34 00 00 00 d3 2f 53 00 00 00 8d b4 0a 00 7e 27 
> 34 00 00 00 d3 2f 53 00 00 00 8d b4 0a 00 7e 27 
> 34 00 00 00 d3 2f 54 00 00 00 a8 bd 0a 00 7e 27 
> 34 00 00 00 d3 2f 55 00 00 00 65 c8 0a 00 7e 27 
> 34 00 00 00 d3 2f 55 00 00 00 65 c8 0a 00 7e 27 
> 34 00 00 00 d3 2f 56 00 00 00 61 d6 0a 00 7e 27 
...
> 34 00 00 00 d3 2f 56 00 00 00 61 d6 0a 00 7e 27 
```

## Characteristic "Indoor Bike Data" from service "Fitness Machine"

see "GATT Specification Supplement 10" chapter "3.125 Indoor Bike Data" page 115

the flag field is 2 bytes long. the length of each parameter is specified in the table under parameters.

each data package is comprised of 2 bytes ( i.e octets ) with *flags** followed by a collection of *parameters**. the number of parameters and the size of each paramater is defined by the flags.

### table *flags*

| Bit | Flag                  |
|-----|-----------------------|
| 0   | More Data             |
| 1   | Average Speed         |
| 2   | Instantaneous Cadence |
| 3   | Average Cadence       |
| 4   | Total Distance        |
| 5   | Resistance Level      |
| 6   | Instantaneous Power   |
| 7   | Average Power         |
| 8   | Expended Energy       |
| 9   | Heart Rate            |
| 10  | Metabolic Equivalent  |
| 11  | Elapsed Time          |
| 12  | Remaining Time        |
| 13  | UNUSED                |
| 14  | UNUSED                |
| 15  | UNUSED                |

### parameters

| Parameter             | Format | Unit       |
|-----------------------|--------|------------|
| Instantaneous Speed   | uint16 | 0.01 Km/h  |
| Average Speed         | uint16 | 0.01 Km/h  |
| Instantaneous Cadence | uint16 | 0.5 /min   |
| Average Cadence       | uint16 | 0.5 /min   |
| Total Distance        | uint24 | metre      |
| Resistance Level      | unit8  | 1 unitless |
| Instantaneous Power   | sint16 | 1 watt     |
| Average Power         | sint16 | 1 watt     |
| Total Energy          | uint16 | kcal       |
| Energy Per Hour       | uint16 | kcal       |
| Energy Per Minute     | uint8  | kcal       |
| Heart Rate            | uint8  | Beats/m    |
| Metabolic Equivalent  | uint8  | 0.1 meta   |
| Elapsed Time          | uint16 | second     |
| Remaining Time        | uint16 | second     |

### example parsing *flags*

```
> 44 00 bc 01 00 00 00 00
```

the first 2 bytes ( i.e `44 00` ) define the flags. according to table *flags* the following features are present: 

```
00100010 // Instantaneous Cadence present, Instantaneous Power present
00000000 // 
```

### example parsing *parameters*

```
> 44 00 17 09 9a 00 73 00 
```

the first 2 bytes show that the features "Instantaneous Cadence" and "Instantaneous Power" are present.

- `44 00` :: flags ( 2 bytes )
- `17 09` :: Instantaneous Cadence ( 2 bytes ) "Unit is 1/2 of a revolution per minute"
- `9a 00` :: Instantaneous Power ( 2 bytes ) "Unit: org.bluetooth.unit.power.watt"
- `73 00` :: @problem(why are there 8 bytes instead of 6?!?)

longer measurements:

```
+++ reading CHARACTERISTIC_INDOOR_BIKE_DATA_N

> 44 00 00 00 00 00 00 00 
...
> 44 00 00 00 00 00 00 00 
> 44 00 83 05 00 00 f0 00 
> 44 00 d5 04 00 00 a1 00 
> 44 00 9a 04 00 00 05 00 
> 44 00 37 04 00 00 00 00 
> 44 00 3c 06 00 00 6e 00 
> 44 00 26 07 56 00 e7 00 
> 44 00 8b 07 6e 00 de 00 
> 44 00 bc 08 7c 00 59 01 
> 44 00 72 09 86 00 bb 01 
> 44 00 e9 09 96 00 c2 01 
> 44 00 10 09 a0 00 f3 00 
> 44 00 49 09 a0 00 50 00 
> 44 00 17 09 9a 00 73 00 
> 44 00 02 09 98 00 6a 00 
> 44 00 44 08 98 00 28 00 
> 44 00 0f 08 98 00 01 00 
> 44 00 c2 07 00 00 00 00 
> 44 00 6f 07 00 00 00 00 
> 44 00 38 07 00 00 00 00 
> 44 00 00 07 00 00 00 00 
> 44 00 a9 06 00 00 00 00 
> 44 00 6d 06 00 00 00 00 
> 44 00 2d 06 00 00 00 00 
> 44 00 dd 05 00 00 00 00 
> 44 00 88 05 00 00 00 00 
> 44 00 2f 05 00 00 00 00 
> 44 00 01 05 00 00 00 00 
> 44 00 a0 04 00 00 00 00 
> 44 00 6c 04 00 00 00 00 
> 44 00 fe 03 00 00 00 00 
> 44 00 c2 03 00 00 00 00 
> 44 00 82 03 00 00 00 00 
> 44 00 3e 03 00 00 00 00 
> 44 00 f3 02 00 00 00 00 
> 44 00 9f 02 00 00 00 00 
> 44 00 3b 02 00 00 00 00 
> 44 00 3b 02 00 00 00 00 
> 44 00 bc 01 00 00 00 00 
> 44 00 00 00 00 00 00 00 
....
> 44 00 00 00 00 00 00 00 
```

## Characteristic "Fitness Machine Feature" from service "Fitness Machine"

see "Fitness Machine Service 1.0: 4.3 Fitness Machine Feature 19" (p19) for details

### Table: Fitness Machine Features ( adapted from Huwai’s documents )

| Bit   | Description                              |
|-------|------------------------------------------|
| 0     | Average Speed Supported                  |
| 1     | Cadence Supported                        |
| 2     | Total Distance Supported                 |
| 3     | Inclination Supported                    |
| 4     | Elevation Gain Supported                 |
| 5     | Pace Supported                           |
| 6     | Step Count Supported                     |
| 7     | Resistance Level Supported               |
| 8     | Stride Count Supported                   |
| 9     | Expended Energy Supported                |
| 10    | Heart Rate Measurement Supported         |
| 11    | Metabolic Equivalent Supported           |
| 12    | Elapsed Time Supported                   |
| 13    | Remaining Time Supported                 |
| 14    | Power Measurement Supported              |
| 15    | Force on Belt and Power Output Supported |
| 16    | User Data Retention Supported            |
| 17-31 | Reserved for Future Use                  |

### Table: Target Setting Features ( adapted from Huwai’s documents )

| Bit   | Description                                                     |
|-------|-----------------------------------------------------------------|
| 0     | Speed Target Setting Supported                                  |
| 1     | Inclination Target Setting Supported                            |
| 2     | Resistance Target Setting Supported                             |
| 3     | Power Target Setting Supported                                  |
| 4     | Heart Rate Target Setting Supported                             |
| 5     | Targeted Expended Energy Configuration Supported                |
| 6     | Targeted Step Number Configuration Supported                    |
| 7     | Targeted Stride Number Configuration Supported                  |
| 8     | Targeted Distance Configuration Supported                       |
| 9     | Targeted Training Time Configuration Supported                  |
| 10    | Targeted Time in Two Heart Rate Zones Configuration Supported   |
| 11    | Targeted Time in Three Heart Rate Zones Configuration Supported |
| 12    | Targeted Time in Five Heart Rate Zones Configuration Supported  |
| 13    | Indoor Bike Simulation Parameters Supported                     |
| 14    | Wheel Circumference Configuration Supported                     |
| 15    | Spin Down Control Supported                                     |
| 16    | Targeted Cadence Configuration Supported                        |
| 17-31 | Reserved for Future Use                                         |

### example CHARACTERISTIC_FITNESS_MACHINE_FEATURE_R

```
+++ reading CHARACTERISTIC_FITNESS_MACHINE_FEATURE_R: 03 40 00 00 0c 60 00 00 

11000000 // Average Speed Supported + Cadence Supported
00000010 // Power Measurement Supported
00000000 
00000000 

00110000 // Resistance Target Setting Supported, Power Target Setting Supported
00000110 // Wheel Circumference Configuration Supported, Spin Down Control Supported
00000000 
00000000 
```

## references
 
- [GATT Specification Supplement 10](https://www.bluetooth.com/specifications/specs/gatt-specification-supplement/) [PDF](https://www.bluetooth.org/docman/handlers/DownloadDoc.ashx?doc_id=570915) ( for "Cycling Power" )
- [Fitness Machine Service 1.0](https://www.bluetooth.com/specifications/specs/fitness-machine-service-1-0/) [PDF](https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=423422)
- [Huawai: Indoor Bike Data](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Guides/ibd-0000001051005923)
- [Arduino BLE Cycling Power Service](https://teaandtechtime.com/arduino-ble-cycling-power-service/)
- [CyclingPowerService.h](https://os.mbed.com/users/p3miya/code/BLE_CyclingPower/file/d9d7edb1ddfc/CyclingPowerService.h/)
