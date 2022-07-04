pio remote run -t upload
Building project locally
Processing megaatmega2560 (platform: atmelavr; board: megaatmega2560; framework: arduino)
----------------------------------------------------------------------------------Verbose mode can be enabled via `-v, --verbose` option
CONFIGURATION: https://docs.platformio.org/page/boards/atmelavr/megaatmega2560.html
PLATFORM: Atmel AVR (4.0.0) > Arduino Mega or Mega 2560 ATmega2560 (Mega 2560)    
HARDWARE: ATMEGA2560 16MHz, 8KB RAM, 248KB Flash
DEBUG: Current (avr-stub) On-board (avr-stub, simavr)
PACKAGES:
 - framework-arduino-avr @ 5.1.0
 - toolchain-atmelavr @ 1.70300.191015 (7.3.0)
Converting main.ino
LDF: Library Dependency Finder -> https://bit.ly/configure-pio-ldf
LDF Modes: Finder ~ chain, Compatibility ~ soft
Found 6 compatible libraries
Scanning dependencies...
Dependency Graph
|-- SPI @ 1.0
|-- Time
Building in release mode
Compiling .pio\build\megaatmega2560\src\main.ino.cpp.o
Linking .pio\build\megaatmega2560\firmware.elf
Checking size .pio\build\megaatmega2560\firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [========= ]  91.2% (used 7472 bytes from 8192 bytes)
Flash: [          ]   3.9% (used 9852 bytes from 253952 bytes)
========================== [SUCCESS] Took 2.72 seconds ==========================
Uploading firmware remotely
Processing megaatmega2560 (platform: atmelavr; board: megaatmega2560; framework: arduino)
--------------------------------------------------------------------------------  
Platform Manager: Installing atmelavr
Downloading...
Unpacking...
Platform Manager: atmelavr@4.0.0 has been installed!
Tool Manager: Installing platformio/framework-arduino-avr @ ~5.1.0
Downloading...
Unpacking...
Tool Manager: framework-arduino-avr@5.1.0 has been installed!
Tool Manager: Installing platformio/tool-avrdude @ ~1.60300.0
Downloading...
Unpacking...
Tool Manager: tool-avrdude@1.60300.200527 has been installed!
Tool Manager: Installing platformio/tool-scons @ ~4.40300.0
Downloading...
Unpacking...
Tool Manager: tool-scons@4.40300.1 has been installed!
Verbose mode can be enabled via `-v, --verbose` option
Configuring upload protocol...
AVAILABLE: wiring
CURRENT: upload_protocol = wiring
Looking for upload port...

Warning! Please install `99-platformio-udev.rules`.
More details: https://docs.platformio.org/en/latest/core/installation/udev-rules.html

Auto-detected: /dev/ttyACM0
Uploading .pio/build/megaatmega2560/firmware.hex

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.01s

avrdude: Device signature = 0x1e9801 (probably m2560)
avrdude: reading input file ".pio/build/megaatmega2560/firmware.hex"
avrdude: writing flash (9852 bytes):

Writing | ################################################## | 100% 1.76s

avrdude: 9852 bytes of flash written
avrdude: verifying flash memory against .pio/build/megaatmega2560/firmware.hex:   
avrdude: load data flash data from input file .pio/build/megaatmega2560/firmware.hex:
avrdude: input file .pio/build/megaatmega2560/firmware.hex contains 9852 bytes    
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 1.28s

avrdude: verifying ...
avrdude: 9852 bytes of flash verified

avrdude done.  Thank you.

========================= [SUCCESS] Took 33.77 seconds =========================  
PS C:\Users\Mikes 6700K\Dropbox\sketchbook\RMS master>








