# Sendungsbewusstsein

see [DOCUMENTATION](./DOCUMENTATION.md) for usage information.

## build prerequisites

### macOS

- install CMake with homebrew `brew install cmake`
- make sure XCode is intalled
- on macOS allow terminal application to use Bluetooth in `System Settings / Privacy & Security / Bluetooth` 

![allow-application-to-use-bluetooth](./assets/allow-application-to-use-bluetooth.png)

*example for macOS 13.4.1 and iTerm*

### Windows

- install [MSYS2](https://www.msys2.org/)
- install [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)
- install the following modules with `pacman` in `MSYS2 UCRT64`:

```
$ pacman -Syu --noconfirm
$ pacman -S --noconfirm mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-cmake git
```

the setup is exclusively for the `MSYS2 UCRT64` branch ( and not for `MSYS2 MINGW64` etcetera ). also it uses `ninja` as a build system instead of `make` ( which is the defaul ton linux + macOS ). this has been tested with Windows 11.

### Linux ( including Raspberry Pi )

on linux ( including Raspberry Pi OS ) install the required packages with [APT](https://en.wikipedia.org/wiki/APT_(software)):

```
$ sudo apt-get update -y
$ sudo apt-get upgrade -y
$ sudo apt-get install git clang cmake pkg-config libdbus-1-dev -y
```

## Cloning with submodules

in order to clone the project with submodules ( i.e `simpleble` library ) use the following command:

```
$ git clone --recurse-submodules https://github.com/dennisppaul/sendungsbewusstsein.git
```

## build 

in the folder containing the `CMakeFile.txt` run the following commands:

```
$ cmake -B build .
$ cmake --build build
$ ./build/sendungsbewusstsein
```

## libraries

this project uses the following libraries:

- [oscpack](https://github.com/RossBencina/oscpack)
- [SimpleBLE](https://github.com/OpenBluetoothToolbox/SimpleBLE)
- [cxxopts](https://github.com/jarro2783/cxxopts)
