# Pinouts diagram to use Bruce
This board can´t use SDCard Sniffer, as used in other boards, because the SDCard CS Pin is physically grounded, so the bus will be always unavailable.

For this board you must solder the MISO/MOSI/SCK pins to the touchscreen SPI line, which can lead to navigation problems, in some ways, so it needs to be tested.

The pins of the XPT2046 that must be connected to CC1101 and NRF24 or other SPI bus, are:
```
      ┌──────────────┐
 VCC ─┤ O            ├─ CLK
     ─┤    /¯¯¯\     ├─
     ─┤   /    XPT   ├─ MOSI
     ─┤   \     /    ├─
     ─┤    \___/     ├─ MISO
 GND ─┤    XPT2046   ├─
     ─┤    ABD8D8    ├─
     ─┤              ├─
      └──────────────┘
```


## USING CUSTOM BOARD, with SPI
| Device  | SCK   | MISO  | MOSI  | CS    | GDO0/CE |
| ---     | :---: | :---: | :---: | :---: | :---:   |
| CC1101  | 1     | 4     | 3     | 15    | 16      |
| NRF24   | 1     | 4     | 3     | 15    | 16      |

| Device  | RX    | TX    | GPIO  |
| ---     | :---: | :---: | :---: |
| GPS     | 44    | 43    | ---   |
| IR RX   |  ---  | ---   | 18    |
| IR TX   |  ---  | ---   | 17    |

FM Radio, PN532 on I2C, other I2C devices
I2C SDA: 17
I2C SCL: 18

GPS Connections
SERIAL_TX: 44
SERIAL_RX: 43

