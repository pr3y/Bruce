#if defined (CORE)
// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include "M5LoRa.h"

// registers
#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_LNA                  0x0c
#define REG_FIFO_ADDR_PTR        0x0d
#define REG_FIFO_TX_BASE_ADDR    0x0e
#define REG_FIFO_RX_BASE_ADDR    0x0f
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_PKT_RSSI_VALUE       0x1a
#define REG_PKT_SNR_VALUE        0x1b
#define REG_MODEM_CONFIG_1       0x1d
#define REG_MODEM_CONFIG_2       0x1e
#define REG_PREAMBLE_MSB         0x20
#define REG_PREAMBLE_LSB         0x21
#define REG_PAYLOAD_LENGTH       0x22
#define REG_MODEM_CONFIG_3       0x26
#define REG_RSSI_WIDEBAND        0x2c
#define REG_DETECTION_OPTIMIZE   0x31
#define REG_DETECTION_THRESHOLD  0x37
#define REG_SYNC_WORD            0x39
#define REG_DIO_MAPPING_1        0x40
#define REG_VERSION              0x42

// modes
#define MODE_LONG_RANGE_MODE 0x80
#define MODE_SLEEP           0x00
#define MODE_STDBY           0x01
#define MODE_TX              0x03
#define MODE_RX_CONTINUOUS   0x05
#define MODE_RX_SINGLE       0x06

// PA config
#define PA_BOOST 0x80

// IRQ masks
#define IRQ_TX_DONE_MASK           0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define IRQ_RX_DONE_MASK           0x40

#define MAX_PKT_LENGTH 255

LoRaClass::LoRaClass()
    : _spiSettings(8E6, MSBFIRST, SPI_MODE0),
      _ss(LORA_DEFAULT_SS_PIN),
      _reset(LORA_DEFAULT_RESET_PIN),
      _dio0(LORA_DEFAULT_DIO0_PIN),
      _frequency(0),
      _packetIndex(0),
      _implicitHeaderMode(0),
      _onReceive(NULL) {
    // overide Stream timeout value
    setTimeout(0);
}

int LoRaClass::begin(long frequency) {
    // setup pins
    pinMode(_ss, OUTPUT);
    // set SS high
    digitalWrite(_ss, HIGH);

    if (_reset != -1) {
        pinMode(_reset, OUTPUT);

        // perform reset
        digitalWrite(_reset, LOW);
        delay(10);
        digitalWrite(_reset, HIGH);
        delay(10);
    }

    // start SPI
    SPI.begin();

    // check version
    uint8_t version = readRegister(REG_VERSION);
    if (version != 0x12) {
        return 0;
    }

    // put in sleep mode
    sleep();

    // set frequency
    setFrequency(frequency);

    // set base addresses
    writeRegister(REG_FIFO_TX_BASE_ADDR, 0);
    writeRegister(REG_FIFO_RX_BASE_ADDR, 0);

    // set LNA boost
    writeRegister(REG_LNA, readRegister(REG_LNA) | 0x03);

    // set auto AGC
    writeRegister(REG_MODEM_CONFIG_3, 0x04);

    // set output power to 17 dBm
    setTxPower(17);

    // put in standby mode
    idle();

    return 1;
}

void LoRaClass::end() {
    // put in sleep mode
    sleep();

    // stop SPI
    SPI.end();
}

int LoRaClass::beginPacket(int implicitHeader) {
    // put in standby mode
    idle();

    if (implicitHeader) {
        implicitHeaderMode();
    } else {
        explicitHeaderMode();
    }

    // reset FIFO address and paload length
    writeRegister(REG_FIFO_ADDR_PTR, 0);
    writeRegister(REG_PAYLOAD_LENGTH, 0);

    return 1;
}

int LoRaClass::endPacket() {
    // put in TX mode
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);

    // wait for TX done
    while ((readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0) {
        yield();
    }

    // clear IRQ's
    writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);

    return 1;
}

int LoRaClass::parsePacket(int size) {
    int packetLength = 0;
    int irqFlags     = readRegister(REG_IRQ_FLAGS);

    if (size > 0) {
        implicitHeaderMode();

        writeRegister(REG_PAYLOAD_LENGTH, size & 0xff);
    } else {
        explicitHeaderMode();
    }

    // clear IRQ's
    writeRegister(REG_IRQ_FLAGS, irqFlags);

    if ((irqFlags & IRQ_RX_DONE_MASK) &&
        (irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0) {
        // received a packet
        _packetIndex = 0;

        // read packet length
        if (_implicitHeaderMode) {
            packetLength = readRegister(REG_PAYLOAD_LENGTH);
        } else {
            packetLength = readRegister(REG_RX_NB_BYTES);
        }

        // set FIFO address to current RX address
        writeRegister(REG_FIFO_ADDR_PTR,
                      readRegister(REG_FIFO_RX_CURRENT_ADDR));

        // put in standby mode
        idle();
    } else if (readRegister(REG_OP_MODE) !=
               (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE)) {
        // not currently in RX mode

        // reset FIFO address
        writeRegister(REG_FIFO_ADDR_PTR, 0);

        // put in single RX mode
        writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_SINGLE);
    }

    return packetLength;
}

int LoRaClass::packetRssi() {
    return (readRegister(REG_PKT_RSSI_VALUE) -
            (_frequency < 868E6 ? 164 : 157));
}

float LoRaClass::packetSnr() {
    return ((int8_t)readRegister(REG_PKT_SNR_VALUE)) * 0.25;
}

size_t LoRaClass::write(uint8_t byte) {
    return write(&byte, sizeof(byte));
}

size_t LoRaClass::write(const uint8_t* buffer, size_t size) {
    int currentLength = readRegister(REG_PAYLOAD_LENGTH);

    // check size
    if ((currentLength + size) > MAX_PKT_LENGTH) {
        size = MAX_PKT_LENGTH - currentLength;
    }

    // write data
    for (size_t i = 0; i < size; i++) {
        writeRegister(REG_FIFO, buffer[i]);
    }

    // update length
    writeRegister(REG_PAYLOAD_LENGTH, currentLength + size);

    return size;
}

int LoRaClass::available() {
    return (readRegister(REG_RX_NB_BYTES) - _packetIndex);
}

int LoRaClass::read() {
    if (!available()) {
        return -1;
    }

    _packetIndex++;

    return readRegister(REG_FIFO);
}

int LoRaClass::peek() {
    if (!available()) {
        return -1;
    }

    // store current FIFO address
    int currentAddress = readRegister(REG_FIFO_ADDR_PTR);

    // read
    uint8_t b = readRegister(REG_FIFO);

    // restore FIFO address
    writeRegister(REG_FIFO_ADDR_PTR, currentAddress);

    return b;
}

void LoRaClass::flush() {
}

void LoRaClass::onReceive(void (*callback)(int)) {
    _onReceive = callback;

    if (callback) {
        pinMode(_dio0, INPUT);

        writeRegister(REG_DIO_MAPPING_1, 0x00);

        attachInterrupt(digitalPinToInterrupt(_dio0), LoRaClass::onDio0Rise,
                        RISING);
    } else {
        detachInterrupt(digitalPinToInterrupt(_dio0));
    }
}

void LoRaClass::receive(int size) {
    if (size > 0) {
        implicitHeaderMode();

        writeRegister(REG_PAYLOAD_LENGTH, size & 0xff);
    } else {
        explicitHeaderMode();
    }

    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
}

void LoRaClass::idle() {
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

void LoRaClass::sleep() {
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

void LoRaClass::setTxPower(int level, int outputPin) {
    if (PA_OUTPUT_RFO_PIN == outputPin) {
        // RFO
        if (level < 0) {
            level = 0;
        } else if (level > 14) {
            level = 14;
        }

        writeRegister(REG_PA_CONFIG, 0x70 | level);
    } else {
        // PA BOOST
        if (level < 2) {
            level = 2;
        } else if (level > 17) {
            level = 17;
        }

        writeRegister(REG_PA_CONFIG, PA_BOOST | (level - 2));
    }
}

void LoRaClass::setFrequency(long frequency) {
    _frequency = frequency;

    uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

    writeRegister(REG_FRF_MSB, (uint8_t)(frf >> 16));
    writeRegister(REG_FRF_MID, (uint8_t)(frf >> 8));
    writeRegister(REG_FRF_LSB, (uint8_t)(frf >> 0));
}

void LoRaClass::setSpreadingFactor(int sf) {
    if (sf < 6) {
        sf = 6;
    } else if (sf > 12) {
        sf = 12;
    }

    if (sf == 6) {
        writeRegister(REG_DETECTION_OPTIMIZE, 0xc5);
        writeRegister(REG_DETECTION_THRESHOLD, 0x0c);
    } else {
        writeRegister(REG_DETECTION_OPTIMIZE, 0xc3);
        writeRegister(REG_DETECTION_THRESHOLD, 0x0a);
    }

    writeRegister(
        REG_MODEM_CONFIG_2,
        (readRegister(REG_MODEM_CONFIG_2) & 0x0f) | ((sf << 4) & 0xf0));
}

void LoRaClass::setSignalBandwidth(long sbw) {
    int bw;

    if (sbw <= 7.8E3) {
        bw = 0;
    } else if (sbw <= 10.4E3) {
        bw = 1;
    } else if (sbw <= 15.6E3) {
        bw = 2;
    } else if (sbw <= 20.8E3) {
        bw = 3;
    } else if (sbw <= 31.25E3) {
        bw = 4;
    } else if (sbw <= 41.7E3) {
        bw = 5;
    } else if (sbw <= 62.5E3) {
        bw = 6;
    } else if (sbw <= 125E3) {
        bw = 7;
    } else if (sbw <= 250E3) {
        bw = 8;
    } else /*if (sbw <= 250E3)*/ {
        bw = 9;
    }

    writeRegister(REG_MODEM_CONFIG_1,
                  (readRegister(REG_MODEM_CONFIG_1) & 0x0f) | (bw << 4));
}

void LoRaClass::setCodingRate4(int denominator) {
    if (denominator < 5) {
        denominator = 5;
    } else if (denominator > 8) {
        denominator = 8;
    }

    int cr = denominator - 4;

    writeRegister(REG_MODEM_CONFIG_1,
                  (readRegister(REG_MODEM_CONFIG_1) & 0xf1) | (cr << 1));
}

void LoRaClass::setPreambleLength(long length) {
    writeRegister(REG_PREAMBLE_MSB, (uint8_t)(length >> 8));
    writeRegister(REG_PREAMBLE_LSB, (uint8_t)(length >> 0));
}

void LoRaClass::setSyncWord(int sw) {
    writeRegister(REG_SYNC_WORD, sw);
}

void LoRaClass::enableCrc() {
    writeRegister(REG_MODEM_CONFIG_2, readRegister(REG_MODEM_CONFIG_2) | 0x04);
}

void LoRaClass::disableCrc() {
    writeRegister(REG_MODEM_CONFIG_2, readRegister(REG_MODEM_CONFIG_2) & 0xfb);
}

byte LoRaClass::random() {
    return readRegister(REG_RSSI_WIDEBAND);
}

void LoRaClass::setPins(int ss, int reset, int dio0) {
    _ss    = ss;
    _reset = reset;
    _dio0  = dio0;
}

void LoRaClass::setSPIFrequency(uint32_t frequency) {
    _spiSettings = SPISettings(frequency, MSBFIRST, SPI_MODE0);
}

void LoRaClass::dumpRegisters(Stream& out) {
    for (int i = 0; i < 128; i++) {
        out.print("0x");
        out.print(i, HEX);
        out.print(": 0x");
        out.println(readRegister(i), HEX);
    }
}

void LoRaClass::explicitHeaderMode() {
    _implicitHeaderMode = 0;

    writeRegister(REG_MODEM_CONFIG_1, readRegister(REG_MODEM_CONFIG_1) & 0xfe);
}

void LoRaClass::implicitHeaderMode() {
    _implicitHeaderMode = 1;

    writeRegister(REG_MODEM_CONFIG_1, readRegister(REG_MODEM_CONFIG_1) | 0x01);
}

void LoRaClass::handleDio0Rise() {
    int irqFlags = readRegister(REG_IRQ_FLAGS);

    // clear IRQ's
    writeRegister(REG_IRQ_FLAGS, irqFlags);

    if ((irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0) {
        // received a packet
        _packetIndex = 0;

        // read packet length
        int packetLength = _implicitHeaderMode
                               ? readRegister(REG_PAYLOAD_LENGTH)
                               : readRegister(REG_RX_NB_BYTES);

        // set FIFO address to current RX address
        writeRegister(REG_FIFO_ADDR_PTR,
                      readRegister(REG_FIFO_RX_CURRENT_ADDR));

        if (_onReceive) {
            _onReceive(packetLength);
        }

        // reset FIFO address
        writeRegister(REG_FIFO_ADDR_PTR, 0);
    }
}

uint8_t LoRaClass::readRegister(uint8_t address) {
    return singleTransfer(address & 0x7f, 0x00);
}

void LoRaClass::writeRegister(uint8_t address, uint8_t value) {
    singleTransfer(address | 0x80, value);
}

uint8_t LoRaClass::singleTransfer(uint8_t address, uint8_t value) {
    uint8_t response;

    digitalWrite(_ss, LOW);

    SPI.beginTransaction(_spiSettings);
    SPI.transfer(address);
    response = SPI.transfer(value);
    SPI.endTransaction();

    digitalWrite(_ss, HIGH);

    return response;
}

void LoRaClass::onDio0Rise() {
    LoRa.handleDio0Rise();
}

LoRaClass LoRa;
#endif