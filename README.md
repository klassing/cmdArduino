# cmdArduino

The Arduino Command Line Interface, aka cmdArduino, is a simple shell that can be run on an Arduino. It's nothing fancy and its main purpose is to allow users to easily call their functions on a running Arduino via a simple serial terminal. It also allows users to pass in arguments from the command line into the functions they wrote so they can easily toggle pins, set blinking speed, set pwm duty cycles, or whatever else might need command line user input. Using it is fairly simple and just requires unzipping the files into the "Arduino/libraries" sub-directory in the Arduino program folder.

- Author: Akiba and Jacinat at FreakLabs
- Copyright (C) 2009 FreakLabs

## Description

This library provides a very simple command line interface exposed via a serial terminal.  Different callback functions can be assigned to specific commands, and different quantities of arguments can be parsed and passed automatically by the library.

It has been tested with Arduino and ESP32 devices.

To see the latest changes to the library please take a look at the [Changelog](https://github.com/klassing/cmdArduino/blob/main/CHANGELOG.md).

## How To Use

To be added later.

### Definition

To be added later.

## License

Copyright (C) 2009 FreakLabs
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3. Neither the name of the the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

Originally written by Christopher Wang aka Akiba.
Please post support questions to the FreakLabs forum.