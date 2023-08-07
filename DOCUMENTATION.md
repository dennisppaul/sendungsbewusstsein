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
  -w, --watchdog [=arg(=2000)]  frequency of watchdog singals in milliseconds. a value of 0 turns 
                                the watchdog off.
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
- `get_characteristic_name`
- `get_feature_name`
- `get_feature_value`
- `set_feature_value`

the following *information* may be sent to the client:

- `send_characteristic_command`
- `send_characteristic_feature_with_value`

### commands ( from client to server )

commands are sent from client to server. commands are used by the user application to interact with the *sendungsbewusstsein CLI application*.

#### scan_for_devices

client requests to update available device list.

```
command .... : scan_for_devices(duration_in_milliseconds)
typetag .... : si
example .... : "scan_for_devices",5000

response ... : command,
               duration_in_milliseconds,
               number_of_devices
typetag .... : sii
example .... : "scan_for_devices",5000,12 
```

#### connect_device

client requests that the server connects to a device. note, that a device can be specified either by *available* device index, name or UUID. further note, that the *available* device index refers to the order of list of scanned devices, whereas the *‌device_index* which is returned in the response refers to the index of the connected devices. 

```
command .... : connect_device(available_device_index/name/UUID)
typetag .... : s(i/s/s)
example .... : "connect_device","Wahoo KICKR" 

response ... : command,
               name/UUID,
               device_index,
               number_of_characteristics
typetag .... : ssii
example .... : "connect_device","Wahoo KICKR",0,4
```

note that on connection some characterics are automatically subscribed to.

#### disconnect_device

client requests that the server disconnects from a device.

```
command .... : disconnect_device(device_index)
typetag .... : si
example .... : "disconnect_device",0 

response ... : command,
               device_index
typetag .... : si
example .... : "disconnect_device",0
```

#### subscribe_to_characteristic

client requests to subscribe to a characteristic of a device.

```
command .... : subscribe_to_characteristic(device_index,
                                           characteristic_index)
typetag .... : sii
example .... : "subscribe_to_characteristic",0,1

response ... : command,
               device_index,
               characteristic_index,
               number_of_features
typetag .... : siii
example .... : "subscribe_to_characteristic",0,1,4
```

#### unsubscribe_from_characteristic

client requests to unsubscribe from a characteristic of a device.

#### get_device_name

client requests device name from server

```
command .... : get_device_name(device_index)
typetag .... : si
example .... : "get_device_name",0 

response ... : command,
               device_index,
               {DEVICE_NAME}
typetag .... : sis
example .... : "get_device_name",0,"WHOOP 4A0934182"
```

#### get_characteristic_name

```
command .... : get_characteristic_name(device_index, 
                                       characteristic_index)
typetag .... : sii
example .... : "get_characteristic_name",0,1

response ... : command,
               device_index,
               characteristic_index,
               characteristic_name
typetag .... : siis
example .... : "get_characteristic_name",0,1,"heartrate_rate_measurement"
```

#### get_characteristic_name

```
command .... : get_characteristic_name(device_index, 
                                       characteristic_index)
typetag .... : sii
example .... : "get_characteristic_name",0,1

response ... : command,
               device_index,
               characteristic_index,
               characteristic_name
typetag .... : siis
example .... : "get_characteristic_name",0,1,"heartrate_rate_measurement"
```

#### get_feature_name

```
command .... : get_feature_name(device_index, 
                                characteristic_index, 
                                feature_index)
typetag .... : siis
example .... : "get_feature_names",0,1,0

response ... : command,
               device_index,
               characteristic_index,
               feature_index,
               feature_name
typetag .... : siiss
example .... : "get_feature_name",0,1,2,"heartrate"
```

#### get_feature_value

client requests to get the value of a feature within a characteristic.

```
command .... : get_feature_value(device_index, 
                                 characteristic_index, 
                                 (feature_index/feature_name))
typetag .... : sii(i/s)
example .... : "get_feature_value",0,1,"heartrate"

response ... : command,
               device_index,
               characteristic_index,
               feature_name,
               feature_value
typetag .... : sii(i/s)f
example .... : "get_feature_value",0,1,"heartrate",123
```

#### set_feature_value

client requests to set the value of a feature within a characteristic.

```
command .... : set_feature_value(device_index, 
                                 characteristic_index, 
                                 (feature_index/feature_name), 
                                 value)
typetag .... : sii(i/s)f
example .... : "set_feature_value",0,1,123

response ... : command,
               device_index,
               characteristic_index,
               feature_name,
               feature_value
typetag .... : sii(i/s)f
example .... : "set_feature_value",0,1,"heartrate",123
```

### information ( from server to client )

*information* are send from server to client ( e.g when a device is connected or a characteristic is subscribed to ).

#### send_characteristic_command

send characteristic command to client.

```
info ....... : send_characteristic_command(device_index,
                                           characteristic_index,
                                           command)
typetag .... : iis
example .... : 0,1,"subscribed"
```

#### send_characteristic_feature_with_value

send value of a feature to client ( often repeating value send from subscribed characteristic ).

```
info ....... : send_characteristic_feature_with_value(device_index,
                                                      characteristic_index,
                                                      feature_name, 
                                                      value)
typetag .... : iisf
example .... : 0,1,"heartrate",52
```
