# Changelog

## Unreleased

**Note:** Unreleased changes are checked in but not part of an official release (available through the Arduino IDE or PlatfomIO) yet. This allows you to test WiP features and give feedback to them.

## [1.1.4] - 2024-04-26
- Miro update to replace SerialConfig datatype with uint32_t for improved compatibility

## [1.1.3] - 2024-01-14
Added the following features:
- Arrow keys LEFT/RIGHT now supported to move the cursor position in the terminal

- Added ability to "insert text" at the cursor position in the terminal

- Added "delete" key functionality

- Added ability to provide a callback function for handling JSON strings (instead of space delimited messages) if desired

## [1.1.2] - 2023-08-01

- Minor update to better handle CRLF based terminals (only process on CR, ignore LF)

## [1.1.1] - 2023-08-01

- Reformat library file/folder structure to comply with latest Arduino standards
- Updated maximum command line input to be 1024 bytes (previously only supported 60 bytes)
    - Note: can be overwritten by pre-compiler statement before including the library
- Increased the number of command line arguments to be parsed up to 128 arguments
    - Note: can be overwritten by pre-compiler statement before including the library
- Updated the temporary buffers (local use only) to be defined by a #define size (instead of by 'magic numbers')

## [1.1.0] - 2022-11-14 - Ryan Klassing

- Add support for DEL or BACKSPACE characters (support for multiple terminals that send different character for backspace)
- Fixed bug where backspace was printed even if the pointer was at the beggining of the command buffer
- Add support for blank command entry (user spams "enter" like we all do on a linux terminal)
- Add pointer to Stream (allows user to pass different Serial streams for HW with multiple serial interfaces)
    - Supports HardwareSerial or SoftwareSerial to be passed
- Re-arrange constructor / begin statements (begin is not mandatory any longer, and is only used to set Stream speed if not previously initialized)
- Add support for a polling 'exit character' to allow calling loops to end the command line interface, if desired

## [1.0.0]

- Forked from [freaklabs cmdArduino v1.0](https://github.com/freaklabs/cmdArduino/releases/tag/v1.0)

## Note

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
