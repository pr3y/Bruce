# Pinouts diagram to use Bruce

## USING CUSTOM BOARD, with SPI
| Device  | SCK   | MISO  | MOSI  | CS    | GDO0/CE   |
| ---     | :---: | :---: | :---: | :---: | :---:     |
| SD Card | 12    | 13    | 11    | 1     | ---       |
| CC1101  | 12    | 13    | 11    | 2     | 21        |
| NRF24   | 12    | 13    | 11    | 10    | 3         |

| Device  | RX    | TX    | GPIO  |
| ---     | :---: | :---: | :---: |
| GPS     | 44    | 43    | ---   |
| IR RX   |  ---  | ---   | 18    |
| IR TX   |  ---  | ---   | 17     |

FM Radio, PN532 on I2C, other I2C devices, shared with GPS Serial interface.
I2C SDA: 44
I2C SCL: 43


## USING THE LILYGO SHIELD (SD_MMC Version),
Configured to be compatible with [Willy's firmware](https://willy-firmware.com/) devices
| Device  | SCK   | MISO  | MOSI  | CS    | GDO0/CE   |
| ---     | :---: | :---: | :---: | :---: | :---:     |
| CC1101  | 43    | 2     | 3     | 1     | 44        |
| NRF24   | 43    | 2     | 3     | 18    | 17        |

| Device  | RX    | TX    | GPIO  |
| ---     | :---: | :---: | :---: |
| GPS     | 13    | 12    | ---   |
| IR RX   |  ---  | ---   | 44 or 11    |
| IR TX   |  ---  | ---   | 10    |

* Ir-RX need a switch to use with Willy board, you can set whatever other pin available
FM Radio, PN532 on I2C, other I2C devices, shared with GPS Serial interface.
I2C SDA: 13
I2C SCL: 12
