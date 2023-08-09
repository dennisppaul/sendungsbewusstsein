# Sendungsbewusstsein / Documentation

## usage

to start the application in terminal mode type:

```
$ ./sendungsbewusstsein
```

once the application has started options can be issued as commands. a full list of commands can be displayed via the help function `--help` or `-h`.

```
sendungsbewusstsein broadcasts BLE devices via OSC.
Usage:
  sendungsbewusstsein [OPTION...] [optional args]

  -h, --help                    print this help message
  -q, --quit                    quit application
  -i, --info                    print capabilities of all available devices
  -s, --scan [=arg(=5000)]      scan for available devices
  -d, --devices arg             specify devices for connect or disconnect. names that contain space 
                                need to be surrounded by quotation marks. indices might change 
                                after scan.
      --connect [=arg(=name)]   connect to device either by name (e.g '--connect=name -p "WHOOP 
                                4A0934182" '), by address (e.g '--connect=address -p 
                                C6FBA-C7E8-0494-34C6-A54DF25AF596') or by index (e.g 
                                '--connect=index -p 3').
      --disconnect [=arg(=name)]
                                disconnect from device either by name, address or index (see 
                                'connect')
      --read arg                read characteristic from device, specified by index. ( e.g '-p 
                                "WHOOP 4A0934182" --read=3' )
      --write arg               write characteristic to device, specified by index. multiple values 
                                must be written in quotation marks and separated by commas. ( e.g 
                                '-p "WHOOP 4A0934182" --write=2' )
      --value arg               spedify values for writing characteristic. multiple values must be 
                                written in quotation marks and separated by commas. ( e.g '-p 
                                "WHOOP 4A0934182" --write=2 --value="0x0A,0x10,0x42"' )

 OSC options:
  -w, --fWatchdog [=arg(=2000)]  frequency of fWatchdog singals in milliseconds. a value of 0 turns 
                                the fWatchdog off.
  -a, --address [=arg(=127.0.0.1)]
                                OSC message transmit address
  -t, --transmit [=arg(=7000)]  port for transmitting messages
  -r, --receive [=arg(=7001)]   port for receiveing messages
  -m, --multicast               enable multicast broadcast
```

the terminal mode can be quit by typing `--quit` or `-q`.

if the full device name is known the application can connect to the device right  away. note, that `--scan` needs to specified in order to collect available devices before connecting:

```
$ ./sendungsbewusstsein --scan=2000 --connect "WHOOP 4A0934182"
```

note, that device names that contain spaces must be surrounded by quotation marks. it is possible to connect to multiple devices with a list of space or comma separated device names:

```
$ ./sendungsbewusstsein --scan=2000 --connect "WHOOP 4A0934182" "Wahoo KICKR 3724"
```

note, there can be no equal sign ( `=` ) after `--connect` or `--disconnect`. it sufficies to specify the first characters of a device name.

once connected, devices can receive and transmit messages via [Open Sound Control](https://en.wikipedia.org/wiki/Open_Sound_Control) (OSC) protocol.

## messages

*‌sendungsbewusstsein* communicates via the OSC protocol. 

in this context *server* is the *sendungsbewusstsein CLI application* and *client* is the user application communicating with the *sendungsbewusstsein CLI application*.

a Bluetooth peripheral is called *device* in this context. devices can have different characteristics that can read from, writting to and/or subscribed to. each characteristic can have one or more features. 

for example: a *Smart Trainer* is a device that has the characteristic *Indoor Bike Data* ( which is part of a *‌Fitness Machine* service, however, services are not interacted with in this context ). the characteristic has the featues *Speed*, *Power* and *Cadence* that can be subscribed to.

all messages whether from *client to server* ( commands ) or *server to client* ( information ) use the *address pattern* `/sendungsbewusstsein/`.

the following *commands* are available:

- `scan_for_devices`
- `connect_device`
- `disconnect_device`
- `subscribe_to_characteristic`
- `unsubscribe_from_characteristic`
- `get_device_name`
- `get_characteristic_name`
- `get_feature_name`
- `get_feature_value`
- `set_feature_value`

the following *information* may be sent to the client:

- `send_device_information`
- `send_device_information_with_value`
- `send_characteristic_information`
- `send_characteristic_feature_with_value`

### commands ( from client to server )

commands are sent from client to server. commands are used by the user application to interact with the *sendungsbewusstsein CLI application*.

#### scan_for_devices

client requests to update available device list.

```
command .... : scan_for_devices(duration_in_milliseconds)
typetag .... : ii
example .... : CMD_SCAN_FOR_DEVICES,5000

response ... : command,
               duration_in_milliseconds,
               number_of_devices
typetag .... : iii
example .... : CMD_SCAN_FOR_DEVICES,5000,12 
```

#### connect_device

client requests that the server connects to a device. note, that a device can be specified either by *available* device index, name or UUID. further note, that the *available* device index refers to the order of list of scanned devices, whereas the *‌device_index* which is returned in the response refers to the index of the connected devices. 

```
command .... : connect_device(available_device_index/name/UUID)
typetag .... : i(i/s/s)
example .... : CMD_CONNECT_DEVICE,"Wahoo KICKR" 

response ... : command,
               available_device_index/name/UUID,
               device_index,
               number_of_characteristics
typetag .... : i(i/s/s)ii
example .... : CMD_CONNECT_DEVICE,"Wahoo KICKR",0,4
```

note that on connection some characterics are automatically subscribed to.

#### disconnect_device

client requests that the server disconnects from a device.

```
command .... : disconnect_device(device_index)
typetag .... : ii
example .... : CMD_DISCONNECT_DEVICE,0 

response ... : command,
               device_index
typetag .... : ii
example .... : CMD_DISCONNECT_DEVICE,0
```

#### subscribe_to_characteristic

client requests to subscribe to a characteristic of a device.

```
command .... : subscribe_to_characteristic(device_index,
                                           characteristic_index)
typetag .... : iii
example .... : CMD_SUBSCRIBE_TO_CHARACTERISTIC,0,1

response ... : command,
               device_index,
               characteristic_index,
               number_of_features
typetag .... : iiii
example .... : CMD_SUBSCRIBE_TO_CHARACTERISTIC,0,1,4
```

#### unsubscribe_from_characteristic

client requests to unsubscribe from a characteristic of a device.

```
command .... : unsubscribe_from_characteristic(device_index,
                                               characteristic_index)
typetag .... : iii
example .... : CMD_UNSUBSCRIBE_FROM_CHARACTERISTIC,0,1

response ... : command,
               device_index,
               characteristic_index
typetag .... : iiii
example .... : CMD_UNSUBSCRIBE_TO_CHARACTERISTIC,0,1
```

#### get_device_name

client requests device name from server

```
command .... : get_device_name(device_index)
typetag .... : ii
example .... : CMD_GET_DEVICE_NAME,0 

response ... : command,
               device_index,
               device_name
typetag .... : iis
example .... : CMD_GET_DEVICE_NAME,0,"WHOOP 4A0934182"
```

#### get_characteristic_name

```
command .... : get_characteristic_name(device_index, 
                                       characteristic_index)
typetag .... : iii
example .... : CMD_GET_CHARACTERISTIC_NAME,0,1

response ... : command,
               device_index,
               characteristic_index,
               characteristic_name
typetag .... : iiis
example .... : CMD_GET_CHARACTERISTIC_NAME,0,1,"heartrate_rate_measurement"
```

#### get_feature_name

```
command .... : get_feature_name(device_index, 
                                characteristic_index, 
                                feature_index)
typetag .... : iiii
example .... : CMD_GET_FEATURE_NAMES,0,1,2

response ... : command,
               device_index,
               characteristic_index,
               feature_index,
               feature_name
typetag .... : iiiis
example .... : CMD_GET_FEATURE_NAMES,0,1,2,"heartrate"
```

#### get_feature_value

client requests to get the value of a feature within a characteristic.

```
command .... : get_feature_value(device_index, 
                                 characteristic_index, 
                                 (feature_index/feature_name))
typetag .... : iii(i/s)
example .... : CMD_GET_FEATURE_VALUE,0,1,"heartrate"

response ... : command,
               device_index,
               characteristic_index,
               feature_name,
               feature_value
typetag .... : iii(i/s)f
example .... : CMD_GET_FEATURE_VALUE,0,1,"heartrate",123
```

#### set_feature_value

client requests to set the value of a feature within a characteristic.

```
command .... : set_feature_value(device_index, 
                                 characteristic_index, 
                                 (feature_index/feature_name), 
                                 value)
typetag .... : iii(i/s)f
example .... : CMD_SET_FEATURE_VALUE,0,1,123

response ... : command,
               device_index,
               characteristic_index,
               feature_name,
               feature_value
typetag .... : iii(i/s)f
example .... : CMD_SET_FEATURE_VALUE,0,1,"heartrate",123
```

### information ( from server to client )

*information* are send from server to client ( e.g when a device is connected or a characteristic is subscribed to ).

#### send_device_information

```
info ....... : send_device_information(device_index,
                                       information)
typetag .... : iii
example .... : INFO_DEVICE,0,CONNECTED_DEVICE
```

#### send_device_information_with_value

```
info ....... : send_device_information_with_value(device_index,
                                                  information, 
                                                  value)
typetag .... : iiii
example .... : INFO_DEVICE_WITH_VALUE,0,NUMBER_OF_SUPPORTED_CHARACTERISTICS,3
```

#### send_characteristic_information

```
info ....... : send_characteristic_information(device_index,
                                               characteristic_index,
                                               information)
typetag .... : iiii
example .... : INFO_CHARACTERISTIC,0,1,UNSUBSCRIBED
```


#### send_characteristic_information_with_value

```
info ....... : send_characteristic_information_with_value(device_index,
                                                          characteristic_index,
                                                          information,
                                                          value)
typetag .... : iiiii
example .... : INFO_CHARACTERISTIC,0,1,SUBSCRIBED,12
```

#### send_characteristic_feature_with_value

send value of a feature to client ( often repeating value send from subscribed characteristic ).

```
info ....... : send_characteristic_feature_with_value(device_index,
                                                      characteristic_index,
                                                      (feature_index/feature_name), 
                                                      value)
typetag .... : iii(i/s)f
example .... : INFO_FEATURE,0,1,"heartrate",52
```

==@TODO see how to best implement both `feature_name` and `feature_index`==
