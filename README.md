# MCU-PLC

MCU-PLC is a proof-of-concept port of [OpenPLC](https://openplcproject.com) core
to microcontrollers (currently ESP32 only). You can run standard Structured
Text programs right on the MCU rather than using the MCU as an external
slave device (standard OpenPLC way).

The core does not contain any communication protocols support for now,
just GPIO-in/out and ADC.

**BEWARE: This is really just a proof of concept. DO NOT USE IT FOR ANYTHING
OTHER THAN RESEARCH!**

## Principle

Similarly to OpenPLC, The Structured Text program is translated into C++ source
files using [Matiec compiler](https://bitbucket.org/mjsousa/matiec). These
files are linked with [OpenPLC
lib](https://github.com/thiagoralves/OpenPLC_v3/tree/master/webserver/core/lib) files
and a very thin MCU-specific layer (also mostly an OpenPLC rip-off).
The resulting binary can be uploaded straight into the MCU. I.e. there's no
API for uploading the ST program, starting and stopping the PLC or anything
like that. Your ST program just runs on your MCU, that's it.

I'm using [the Arduino framework](https://docs.platformio.org/en/latest/frameworks/arduino.html#framework-arduino)
but the code is very easily portable to any other PlatformIO-supported framework
(see `hal.cpp` file).

## Requirements

You need:
- standard `build-essential` or equivalent packages to build Matiec
- working [PlatformIO](https://platformio.org/) installation

The code is tested on the `NodeMCU ESP-32S` board but it should work
on every ESP32 board with small or no modifications. You can change
the board type by changing `board` option in the platformio.ini file
(see [PlatfrmIO docs](https://docs.platformio.org/en/latest/platforms/espressif32.html#boards)).
Pin mapping is defined in the [src/hal.cpp](src/hal.cpp) file (`..._PINS[]` arrays).

## Preparation

Get OpenPLC sources:
```shell
$ git submodule init
$ git submodule update
```

Compile `matiec` (IEC 61131-3 compiler):
```shell
$ cd OpenPLC_v3/utils/matiec_src/
$ autoreconf -i
$ ./configure
$ make
$ cd ../../..
```

## PLC Program Compilation

At first, generate PLC sources (using `matiec`) using:
```shell
$ make
```

This will generate files from the default `blink.st` program. You can compile
different program using:
```shell
$ make PLC_SRC_FILE=programs/blink2.st
```

All PLC C sources are ready now, you can connect your ESP32 to your PC and build
and upload the program using PlatformIO:
```shell
$ pio run -t upload
```

If everything went well, you should see blinking LED on your ESP32 now.

## Running on Host (Linux only)

To make debugging more convenient, you can run your PLC straight on the host
computer, if it's a Linux machine:

```shell
$ pio run -e linux_x86_64
$ .pio/build/linux_x86_64/program
```