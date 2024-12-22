```
.
├── platformio.ini
├── boards
    ├── [board]
    │   └── interface.cpp
    ├── pinouts
    │   ├── pins_arduino.h
    │   └── [board].h
    ├── [board].json
    └── [board].ini
...
```

# Files
(Replace \[board] with the board name)

## boards/pinouts/pins_arduio.h
This is where you will put the flag that will include your boards pinouts header.

## boards/pinouts/\[board].h
This is where you put the flags and pinouts to the board. Look at other boards for whats needed.
Here is an official example and what we are actually using here:
https://github.com/espressif/arduino-esp32/blob/master/variants/esp32s3/pins_arduino.h

## boards/\[board]/interface.cpp
This is where you do the board specific setup code

## boards/\[board].json
This is the board config. Look at other boards for whats needed.
Here is an offical example and what we are actually using here:
https://github.com/platformio/platform-espressif32/blob/master/boards/esp32-s3-devkitc-1.json

## boards/\[board].ini
This is the platformio config for the device. Look at other boards for whats needed.