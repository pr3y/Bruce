# Pinouts diagram to use Bruce

| Device  | SCK   | MISO  | MOSI  | CS    | GDO0/CE   |
| ---     | :---: | :---: | :---: | :---: | :---:     |
| SD Card | 25    | 27    | 26    | 33    | ---       |
| CC1101  | 25    | 27    | 26    | 32    | 39        |
| NRF24   | 25    | 27    | 26    | 38    | 37        |

| Device  | RX    | TX    | GPIO  |
| ---     | :---: | :---: | :---: |
| GPS     | 13    | 12    | ---   |
| CH9329  | 13    | 12    | ---   |
| IR RX   |  ---  | ---   | 15    |
| IR TX   |  ---  | ---   | 2     |

FM Radio, PN532 on I2C, other I2C devices
I2C SDA: 21
I2C SCL: 22


GPIO 36 and 17 are free to use whatever you need ,such as FS1000A and the other single pinned modules, or IR LED and receiver
