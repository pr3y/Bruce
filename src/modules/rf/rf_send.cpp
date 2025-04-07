#include "rf_send.h"
#include "core/type_convertion.h"
#include "rf_utils.h"
#include <RCSwitch.h>

void sendRfCommand(struct RfCodes rfcode) {
    uint32_t frequency = rfcode.frequency;
    String protocol = rfcode.protocol;
    String preset = rfcode.preset;
    String data = rfcode.data;
    uint64_t key = rfcode.key;
    byte modulation = 2; // possible values for CC1101: 0 = 2-FSK, 1 =GFSK, 2=ASK, 3 = 4-FSK, 4 = MSK
    float deviation = 1.58;
    float rxBW = 270.83; // Receive bandwidth
    float dataRate = 10; // Data Rate
                         /*
                             Serial.println("sendRawRfCommand");
                             Serial.println(data);
                             Serial.println(frequency);
                             Serial.println(preset);
                             Serial.println(protocol);
                           */

    // Radio preset name (configures modulation, bandwidth, filters, etc.).
    /*  supported flipper presets:
        FuriHalSubGhzPresetIDLE, // < default configuration
        FuriHalSubGhzPresetOok270Async, ///< OOK, bandwidth 270kHz, asynchronous
        FuriHalSubGhzPresetOok650Async, ///< OOK, bandwidth 650kHz, asynchronous
        FuriHalSubGhzPreset2FSKDev238Async, //< FM, deviation 2.380371 kHz, asynchronous
        FuriHalSubGhzPreset2FSKDev476Async, //< FM, deviation 47.60742 kHz, asynchronous
        FuriHalSubGhzPresetMSK99_97KbAsync, //< MSK, deviation 47.60742 kHz, 99.97Kb/s, asynchronous
        FuriHalSubGhzPresetGFSK9_99KbAsync, //< GFSK, deviation 19.042969 kHz, 9.996Kb/s, asynchronous
        FuriHalSubGhzPresetCustom, //Custom Preset
    */
    // struct Protocol rcswitch_protocol;
    int rcswitch_protocol_no = 1;
    if (preset == "FuriHalSubGhzPresetOok270Async") {
        rcswitch_protocol_no = 1;
        //  pulseLength , syncFactor , zero , one, invertedSignal
        // rcswitch_protocol = { 350, {  1, 31 }, {  1,  3 }, {  3,  1 }, false };
        modulation = 2;
        rxBW = 270;
    } else if (preset == "FuriHalSubGhzPresetOok650Async") {
        rcswitch_protocol_no = 2;
        // rcswitch_protocol = { 650, {  1, 10 }, {  1,  2 }, {  2,  1 }, false };
        modulation = 2;
        rxBW = 650;
    } else if (preset == "FuriHalSubGhzPreset2FSKDev238Async") {
        modulation = 0;
        deviation = 2.380371;
        rxBW = 238;
    } else if (preset == "FuriHalSubGhzPreset2FSKDev476Async") {
        modulation = 0;
        deviation = 47.60742;
        rxBW = 476;
    } else if (preset == "FuriHalSubGhzPresetMSK99_97KbAsync") {
        modulation = 4;
        deviation = 47.60742;
        dataRate = 99.97;
    } else if (preset == "FuriHalSubGhzPresetGFSK9_99KbAsync") {
        modulation = 1;
        deviation = 19.042969;
        dataRate = 9.996;
    } else {
        bool found = false;
        for (int p = 0; p < 30; p++) {
            if (preset == String(p)) {
                rcswitch_protocol_no = preset.toInt();
                found = true;
            }
        }
        if (!found) {
            Serial.print("unsupported preset: ");
            Serial.println(preset);
            return;
        }
    }

    // init transmitter
    if (!initRfModule("", frequency / 1000000.0)) return;
    if (bruceConfig.rfModule == CC1101_SPI_MODULE) { // CC1101 in use
        // derived from
        // https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/blob/master/examples/Rc-Switch%20examples%20cc1101/SendDemo_cc1101/SendDemo_cc1101.ino
        ELECHOUSE_cc1101.setModulation(modulation);
        if (deviation) ELECHOUSE_cc1101.setDeviation(deviation);
        if (rxBW)
            ELECHOUSE_cc1101.setRxBW(rxBW
            ); // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
        if (dataRate) ELECHOUSE_cc1101.setDRate(dataRate);
        pinMode(bruceConfig.CC1101_bus.io0, OUTPUT);
        ELECHOUSE_cc1101.setPA(12
        ); // set TxPower. The following settings are possible depending on the frequency band.  (-30  -20 -15
           // -10  -6    0    5    7    10   11   12)   Default is max!
        ELECHOUSE_cc1101.SetTx();
    } else {
        // other single-pinned modules in use
        if (modulation != 2) {
            Serial.print("unsupported modulation: ");
            Serial.println(modulation);
            return;
        }
        initRfModule("tx", frequency / 1000000.0);
    }

    if (protocol == "RAW") {
        // count the number of elements of RAW_Data
        int buff_size = 0;
        int index = 0;
        while (index >= 0) {
            index = data.indexOf(' ', index + 1);
            buff_size++;
        }
        // alloc buffer for transmittimings
        int *transmittimings =
            (int *)calloc(sizeof(int), buff_size + 1); // should be smaller the data.length()
        size_t transmittimings_idx = 0;

        // split data into words, convert to int, and store them in transmittimings
        int startIndex = 0;
        index = 0;
        for (transmittimings_idx = 0; transmittimings_idx < buff_size; transmittimings_idx++) {
            index = data.indexOf(' ', startIndex);
            if (index == -1) {
                transmittimings[transmittimings_idx] = data.substring(startIndex).toInt();
            } else {
                transmittimings[transmittimings_idx] = data.substring(startIndex, index).toInt();
            }
            startIndex = index + 1;
        }
        transmittimings[transmittimings_idx] = 0; // termination

        // send rf command
        displayTextLine("Sending..");
        RCSwitch_RAW_send(transmittimings);
        free(transmittimings);
    } else if (protocol == "BinRAW") {
        // transform from "00 01 02 ... FF" into "00000000 00000001 00000010 .... 11111111"
        rfcode.data = hexStrToBinStr(rfcode.data);
        // Serial.println(rfcode.data);
        rfcode.data.trim();
        RCSwitch_RAW_Bit_send(rfcode);
    }

    else if (protocol == "RcSwitch") {
        data.replace(" ", ""); // remove spaces
        // uint64_t data_val = strtoul(data.c_str(), nullptr, 16);
        uint64_t data_val = rfcode.key;
        int bits = rfcode.Bit;
        int pulse = rfcode.te; // not sure about this...
        int repeat = 10;
        /*
        Serial.print("RcSwitch: ");
        Serial.println(data_val,16);
        Serial.println(bits);
        Serial.println(pulse);
        Serial.println(rcswitch_protocol_no);
        */
        displayTextLine("Sending..");
        RCSwitch_send(data_val, bits, pulse, rcswitch_protocol_no, repeat);
    } else if (protocol.startsWith("Princeton")) {
        RCSwitch_send(rfcode.key, rfcode.Bit, 350, 1, 10);
    } else {
        Serial.print("unsupported protocol: ");
        Serial.println(protocol);
        Serial.println("Sending RcSwitch 11 protocol");
        // if(protocol.startsWith("CAME") || protocol.startsWith("HOLTEC" || NICE)) {
        RCSwitch_send(rfcode.key, rfcode.Bit, 270, 11, 10);
        //}

        return;
    }

    // digitalWrite(bruceConfig.rfTx, LED_OFF);
    deinitRfModule();
}

void RCSwitch_send(uint64_t data, unsigned int bits, int pulse, int protocol, int repeat) {
    // derived from
    // https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/blob/master/examples/Rc-Switch%20examples%20cc1101/SendDemo_cc1101/SendDemo_cc1101.ino

    RCSwitch mySwitch = RCSwitch();

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) {
        mySwitch.enableTransmit(bruceConfig.CC1101_bus.io0);
    } else {
        mySwitch.enableTransmit(bruceConfig.rfTx);
    }

    mySwitch.setProtocol(protocol); // override
    if (pulse) { mySwitch.setPulseLength(pulse); }
    mySwitch.setRepeatTransmit(repeat);
    mySwitch.send(data, bits);

    /*
    Serial.println(data,HEX);
    Serial.println(bits);
    Serial.println(pulse);
    Serial.println(protocol);
    Serial.println(repeat);
    */

    mySwitch.disableTransmit();

    deinitRfModule();
}

// ported from https://github.com/sui77/rc-switch/blob/3a536a172ab752f3c7a58d831c5075ca24fd920b/RCSwitch.cpp
void RCSwitch_RAW_Bit_send(RfCodes data) {
    int nTransmitterPin = bruceConfig.rfTx;
    if (bruceConfig.rfModule == CC1101_SPI_MODULE) { nTransmitterPin = bruceConfig.CC1101_bus.io0; }

    if (data.data == "") return;
    bool currentlogiclevel = false;
    int nRepeatTransmit = 1;
    for (int nRepeat = 0; nRepeat < nRepeatTransmit; nRepeat++) {
        int currentBit = data.data.length();
        while (currentBit >= 0) { // Starts from the end of the string until the max number of bits to send
            char c = data.data[currentBit];
            if (c == '1') {
                currentlogiclevel = true;
            } else if (c == '0') {
                currentlogiclevel = false;
            } else {
                Serial.println("Invalid data");
                currentBit--;
                continue;
                // return;
            }

            digitalWrite(nTransmitterPin, currentlogiclevel ? HIGH : LOW);
            delayMicroseconds(data.te);

            // Serial.print(currentBit);
            // Serial.print("=");
            // Serial.println(currentlogiclevel);

            currentBit--;
        }
        digitalWrite(nTransmitterPin, LOW);
    }
}

void RCSwitch_RAW_send(int *ptrtransmittimings) {
    int nTransmitterPin = bruceConfig.rfTx;
    if (bruceConfig.rfModule == CC1101_SPI_MODULE) { nTransmitterPin = bruceConfig.CC1101_bus.io0; }

    if (!ptrtransmittimings) return;

    bool currentlogiclevel = true;
    int nRepeatTransmit = 1; // repeats RAW signal twice!
    // HighLow pulses ;

    for (int nRepeat = 0; nRepeat < nRepeatTransmit; nRepeat++) {
        unsigned int currenttiming = 0;
        while (ptrtransmittimings[currenttiming]) { // && currenttiming < RCSWITCH_MAX_CHANGES
            if (ptrtransmittimings[currenttiming] >= 0) {
                currentlogiclevel = true;
            } else {
                // negative value
                currentlogiclevel = false;
                ptrtransmittimings[currenttiming] = (-1) * ptrtransmittimings[currenttiming]; // invert sign
            }

            digitalWrite(nTransmitterPin, currentlogiclevel ? HIGH : LOW);
            delayMicroseconds(ptrtransmittimings[currenttiming]);

            /*
            Serial.print(ptrtransmittimings[currenttiming]);
            Serial.print("=");
            Serial.println(currentlogiclevel);
            */

            currenttiming++;
        }
        digitalWrite(nTransmitterPin, LOW);
    } // end for
}
