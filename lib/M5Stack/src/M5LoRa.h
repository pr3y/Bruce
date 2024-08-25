#if defined (CORE)
// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

// 23FEB18 gojimmypi reversed LORA_DEFAULT_RESET_PIN and LORA_DEFAULT_DIO0_PIN
// pin numbers reset on pin 36; irq on pin 26

// 12NOV2020 Zontex reversed LORA_DEFAULT_RESET_PIN and LORA_DEFAULT_DIO0_PIN
// pin numbers reset on pin 26; irq on pin 36

#ifndef M5LORA_H
#define M5LORA_H

#include <Arduino.h>
#include <SPI.h>

#define LORA_DEFAULT_SS_PIN    5
#define LORA_DEFAULT_RESET_PIN 26
#define LORA_DEFAULT_DIO0_PIN  36

#define PA_OUTPUT_RFO_PIN      0
#define PA_OUTPUT_PA_BOOST_PIN 1

class LoRaClass : public Stream {
   public:
    LoRaClass();

    int begin(long frequency);
    void end();

    int beginPacket(int implicitHeader = false);
    int endPacket();

    int parsePacket(int size = 0);
    int packetRssi();
    float packetSnr();

    // from Print
    virtual size_t write(uint8_t byte);
    virtual size_t write(const uint8_t* buffer, size_t size);

    // from Stream
    virtual int available();
    virtual int read();
    virtual int peek();
    virtual void flush();

    void onReceive(void (*callback)(int));

    void receive(int size = 0);
    void idle();
    void sleep();

    void setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
    void setFrequency(long frequency);
    void setSpreadingFactor(int sf);
    void setSignalBandwidth(long sbw);
    void setCodingRate4(int denominator);
    void setPreambleLength(long length);
    void setSyncWord(int sw);
    void enableCrc();
    void disableCrc();

    // deprecated
    void crc() {
        enableCrc();
    }
    void noCrc() {
        disableCrc();
    }

    byte random();

    void setPins(int ss    = LORA_DEFAULT_SS_PIN,
                 int reset = LORA_DEFAULT_RESET_PIN,
                 int dio0  = LORA_DEFAULT_DIO0_PIN);
    void setSPIFrequency(uint32_t frequency);

    void dumpRegisters(Stream& out);

   private:
    void explicitHeaderMode();
    void implicitHeaderMode();

    void handleDio0Rise();

    uint8_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint8_t value);
    uint8_t singleTransfer(uint8_t address, uint8_t value);

    static void onDio0Rise();

   private:
    SPISettings _spiSettings;
    int _ss;
    int _reset;
    int _dio0;
    int _frequency;
    int _packetIndex;
    int _implicitHeaderMode;
    void (*_onReceive)(int);
};

extern LoRaClass LoRa;

#endif
#endif