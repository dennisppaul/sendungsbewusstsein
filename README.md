# sendungsbewusstsein

## usage + build prerequisites

- install CMake with homebrew `brew install cmake`
- on macOS allow terminal application to use Bluetooth in `System Settings / Privacy & Security / Bluetooth` 

![allow-application-to-use-bluetooth](./assets/allow-application-to-use-bluetooth.png)

*example for macOS 13.4.1 and iTerm*

## usage

to simply start the application in terminal mode type:

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
  -p, --peripherals arg         specify peripherals for connect or disconnect. names that contain 
                                space need to be surrounded by quotation marks. indices might 
                                change after scan.
      --connect [=arg(=name)]   connect to device either by name (e.g '-p "WHOOP 4A0934182" 
                                --connect=name'), by address (e.g '-p 
                                C6FBA-C7E8-0494-34C6-A54DF25AF596 --connect=address') or by index 
                                (e.g '-p 3 --connect=index').
      --disconnect [=arg(=name)]
                                disconnect from device either by name, address or index (see 
                                'connect')

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

note, that can be no equal sign ( `=` ) after `--connect` or `--disconnect`.

once connected, devices start transmitting information via OSC.

## build 

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## libraries

this project uses the following libraries:

- [oscpack](https://github.com/RossBencina/oscpack)
- [SimpleBLE](https://github.com/OpenBluetoothToolbox/SimpleBLE)
- [cxxopts](https://github.com/jarro2783/cxxopts)
