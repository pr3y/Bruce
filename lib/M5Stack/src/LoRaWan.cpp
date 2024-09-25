#if defined (CORE)
/*
  LoRaWAN.cpp for M5Stack (fork from
  https://github.com/toddkrein/OTAA-LoRaWAN-Seeed)

  2013 Copyright (c) Seeed Technology Inc.  All right reserved.
  2017 Copyright (c) Todd Krein. All rights reserved.

  Original Author: Wayne Weng
  Date: 2016-10-17

  Greatly overhauled 2017 by Todd Krein (todd@krein.org)

  The MIT License (MIT)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.1  USA
*/

#include "LoRaWan.h"

const char *physTypeStr[10] = {"EU434",        "EU868", "US915", "US915HYBRID",
                               "AU915",        "AS923", "CN470", "KR920",
                               "CN470PREQUEL", "STE920"};

LoRaWanClass::LoRaWanClass(void) {
    memset(_buffer, 0, 256);
    debug = false;
}

void LoRaWanClass::init(void) {
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    // Serial2.begin(9600, SERIAL_8N1, 2, 5);
    // SerialLoRa.begin(9600);
}

void LoRaWanClass::getVersion(char *buffer, short length,
                              unsigned char timeout) {
    if (buffer) {
        while (SerialLoRa.available()) SerialLoRa.read();
        sendCommand("AT+VER=?\r\n");
        readBuffer(buffer, length, timeout);
    }
}

void LoRaWanClass::getId(char *buffer, short length, unsigned char timeout) {
    if (buffer) {
        while (SerialLoRa.available()) SerialLoRa.read();
        sendCommand("AT+ID=?\r\n");
        readBuffer(buffer, length, timeout);
    }
}

void LoRaWanClass::setId(char *DevAddr, char *DevEUI, char *AppEUI) {
    char cmd[64];

    if (DevAddr) {
        memset(cmd, 0, 64);
        sprintf(cmd, "AT+ID=DevAddr,\"%s\"\r\n", DevAddr);
        sendCommand(cmd);
        delay(DEFAULT_TIMEWAIT);
    }

    if (DevEUI) {
        memset(cmd, 0, 64);
        sprintf(cmd, "AT+ID=DevEui,\"%s\"\r\n", DevEUI);
        sendCommand(cmd);
        delay(DEFAULT_TIMEWAIT);
    }

    if (AppEUI) {
        memset(cmd, 0, 64);
        sprintf(cmd, "AT+ID=AppEui,\"%s\"\r\n", AppEUI);
        sendCommand(cmd);
        delay(DEFAULT_TIMEWAIT);
    }
}

void LoRaWanClass::setKey(char *NwkSKey, char *AppSKey, char *AppKey) {
    char cmd[64];

    if (NwkSKey) {
        memset(cmd, 0, 64);
        sprintf(cmd, "AT+KEY=NWKSKEY,\"%s\"\r\n", NwkSKey);
        sendCommand(cmd);
#if _DEBUG_SERIAL_
        loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
        delay(DEFAULT_TIMEWAIT);
    }

    if (AppSKey) {
        memset(cmd, 0, 64);
        sprintf(cmd, "AT+KEY=APPSKEY,\"%s\"\r\n", AppSKey);
        sendCommand(cmd);
#if _DEBUG_SERIAL_
        loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
        delay(DEFAULT_TIMEWAIT);
    }

    if (AppKey) {
        memset(cmd, 0, 64);
        sprintf(cmd, "AT+KEY= APPKEY,\"%s\"\r\n", AppKey);
        sendCommand(cmd);
#if _DEBUG_SERIAL_
        loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
        delay(DEFAULT_TIMEWAIT);
    }
}

bool LoRaWanClass::setDataRate(_data_rate_t dataRate,
                               _physical_type_t physicalType) {
    char cmd[32];
    // const char *str;

    if ((physicalType <= UNINIT) && (physicalType >= UNDEF)) {
        myType = UNINIT;
        debugPrint("Unknown datarate\n");
        return false;
    }

    myType = physicalType;
    sendCommand(F("AT+DR="));
    //    str = (const char*)(physTypeStr[(int)myType]);
    sendCommand(physTypeStr[myType]);
    //    sendCommand(str);
    sendCommand(F("\r\n"));
//    if(physicalType == EU434)sendCommand("AT+DR=EU433\r\n");
//    else if(physicalType == EU868)sendCommand("AT+DR=EU868\r\n");
//    else if(physicalType == US915)sendCommand("AT+DR=US915\r\n");
//    else if(physicalType == AU920)sendCommand("AT+DR=AU920\r\n");
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);

    memset(cmd, 0, 32);
    sprintf(cmd, "AT+DR=%d\r\n", dataRate);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
    return true;
}

void LoRaWanClass::setPower(short power) {
    char cmd[32];

    memset(cmd, 0, 32);
    sprintf(cmd, "AT+POWER=%d\r\n", power);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setPort(unsigned char port) {
    char cmd[32];

    memset(cmd, 0, 32);
    sprintf(cmd, "AT+PORT=%d\r\n", port);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setAdaptiveDataRate(bool command) {
    if (command)
        sendCommand("AT+ADR=ON\r\n");
    else
        sendCommand("AT+ADR=OFF\r\n");
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::getChannel(void) {
    sendCommand("AT+CH\r\n");

    loraDebugPrint(DEFAULT_DEBUGTIME);

    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setChannel(unsigned char channel, float frequency) {
    char cmd[32];

    //    if(channel > 16) channel = 16;      // ??? this is wrong for US915

    memset(cmd, 0, 32);
    if (frequency == 0)
        sprintf(cmd, "AT+CH=%d,0\r\n", channel);
    else
        sprintf(cmd, "AT+CH=%d,%d.%d\r\n", channel, (short)frequency,
                short(frequency * 10) % 10);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setChannel(unsigned char channel, float frequency,
                              _data_rate_t dataRata) {
    char cmd[32];

    if (channel > 16) channel = 16;

    memset(cmd, 0, 32);
    sprintf(cmd, "AT+CH=%d,%d.%d,%d\r\n", channel, (short)frequency,
            short(frequency * 10) % 10, dataRata);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setChannel(unsigned char channel, float frequency,
                              _data_rate_t dataRataMin,
                              _data_rate_t dataRataMax) {
    char cmd[32];

    if (channel > 16) channel = 16;

    memset(cmd, 0, 32);
    sprintf(cmd, "AT+CH=%d,%d.%d,%d,%d\r\n", channel, (short)frequency,
            short(frequency * 10) % 10, dataRataMin, dataRataMax);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

bool LoRaWanClass::transferPacket(char *buffer, unsigned char timeout) {
    unsigned char length = strlen(buffer);
    int count;

    while (SerialLoRa.available()) SerialLoRa.read();

    sendCommand("AT+MSG=\"");
    for (int i = 0; i < length; i++) SerialLoRa.write(buffer[i]);
    sendCommand("\"\r\n");

    while (true) {
        memset(_buffer, 0, BEFFER_LENGTH_MAX);
        count = readLine(_buffer, BEFFER_LENGTH_MAX, timeout);
        if (count == 0) continue;
            // handle timout!
#if _DEBUG_SERIAL_
        SerialUSB.print(_buffer);
#endif
        if (strstr(_buffer, "+MSG: Done")) return true;
    }

    //    memset(_buffer, 0, BEFFER_LENGTH_MAX);
    //    readBuffer(_buffer, BEFFER_LENGTH_MAX, timeout);
    //#if _DEBUG_SERIAL_
    //    SerialUSB.print(_buffer);
    //#endif
    //    if(strstr(_buffer, "+MSG: Done"))return true;
    //    return false;
}

bool LoRaWanClass::transferPacket(unsigned char *buffer, unsigned char length,
                                  unsigned char timeout) {
    char temp[3] = {0};

    while (SerialLoRa.available()) SerialLoRa.read();

    sendCommand("AT+MSGHEX=\"");
    for (int i = 0; i < length; i++) {
        sprintf(temp, "%02x", buffer[i]);
        SerialLoRa.write(temp);
    }
    sendCommand("\"\r\n");

    memset(_buffer, 0, BEFFER_LENGTH_MAX);
    readBuffer(_buffer, BEFFER_LENGTH_MAX, timeout);
#if _DEBUG_SERIAL_
    SerialUSB.print(_buffer);
#endif
    if (strstr(_buffer, "+MSGHEX: Done")) return true;
    return false;
}

bool LoRaWanClass::transferPacketWithConfirmed(char *buffer,
                                               unsigned char timeout) {
    unsigned char length = strlen(buffer);
    int i;
    bool sentOK;

    sentOK = false;

    while (SerialLoRa.available()) SerialLoRa.read();

    sendCommand("AT+CMSG=\"");
    for (int i = 0; i < length; i++) SerialLoRa.write(buffer[i]);
    sendCommand("\"\r\n");

#ifdef deadcode
    memset(_buffer, 0, BEFFER_LENGTH_MAX);
    i          = readBuffer(_buffer, BEFFER_LENGTH_MAX, timeout);
    _buffer[i] = 0;
#if _DEBUG_SERIAL_
    SerialUSB.print(_buffer);
#endif
    if (strstr(_buffer, "+CMSG: ACK Received"))
        return true;
    else
        return false;
#endif

    while (true) {
        memset(_buffer, 0, BEFFER_LENGTH_MAX);
        i = readLine(_buffer, BEFFER_LENGTH_MAX, timeout);
        if (i == 0) continue;
        _buffer[i] = 0;

        // !!! handle timeout
#if _DEBUG_SERIAL_
        SerialUSB.print(_buffer);
#endif
        if (strstr(_buffer, "+CMSG: Start")) continue;
        if (strstr(_buffer, "+CMSG: Wait ACK")) continue;
        if (strstr(_buffer, "+CMSG: TX")) continue;
        if (strstr(_buffer, "+CMSG: RXWIN")) continue;
        if (strstr(_buffer, "+CMSG: No free channel")) break;
        if (strstr(_buffer, "+CMSG: Done")) break;

        if (strstr(_buffer, "+CMSG: ACK Received")) {
            sentOK = true;
            continue;
        }
        SerialUSB.print("Result didn't match anything I expected.\n");
    }

    return sentOK;
}

bool LoRaWanClass::transferPacketWithConfirmed(unsigned char *buffer,
                                               unsigned char length,
                                               unsigned char timeout) {
    char temp[3] = {0};
    int i;
    unsigned char *ptr;

    while (SerialLoRa.available()) SerialLoRa.read();

    sendCommand("AT+CMSGHEX=\"");
    for (int i = 0; i < length; i++) {
        sprintf(temp, "%02x", buffer[i]);
        SerialLoRa.write(temp);
    }
    sendCommand("\"\r\n");
#if _DEBUG_SERIAL_
    ptr = buffer;
    for (i = 0; i < length; i++) SerialUSB.print(*(ptr++));

    SerialUSB.println("");
#endif

    memset(_buffer, 0, BEFFER_LENGTH_MAX);
    i          = readBuffer(_buffer, BEFFER_LENGTH_MAX, timeout);
    _buffer[i] = 0;

    SerialUSB.print(_buffer);

    if (strstr(_buffer, "+CMSGHEX: ACK Received"))
        return true;
    else
        return false;
}

short LoRaWanClass::receivePacket(char *buffer, short length, short *rssi) {
    char *ptr;
    short number = 0;

    ptr = strstr(_buffer, "RSSI ");
    if (ptr)
        *rssi = atoi(ptr + 5);
    else
        *rssi = -255;

    ptr = strstr(_buffer, "RX: \"");
    if (ptr) {
        ptr += 5;
        for (short i = 0;; i++) {
            char temp[3]      = {0, 0};
            unsigned char tmp = '?', result = 0;

            temp[0] = *(ptr + i * 3);
            temp[1] = *(ptr + i * 3 + 1);

            for (unsigned char j = 0; j < 2; j++) {
                if ((temp[j] >= '0') && (temp[j] <= '9'))
                    tmp = temp[j] - '0';
                else if ((temp[j] >= 'A') && (temp[j] <= 'F'))
                    tmp = temp[j] - 'A' + 10;
                else if ((temp[j] >= 'a') && (temp[j] <= 'f'))
                    tmp = temp[j] - 'a' + 10;

                result = result * 16 + tmp;
            }

            if (i < length) buffer[i] = result;

            if (*(ptr + i * 3 + 3) == '\"' && *(ptr + i * 3 + 4) == '\r' &&
                *(ptr + i * 3 + 5) == '\n') {
                number = i + 1;
                break;
            }
        }
    }

    ptr = strstr(_buffer, "MACCMD: \"");
    if (ptr) {
        buffer[0] = 'M';
        buffer[1] = 'A';
        buffer[2] = 'C';
        buffer[3] = 'C';
        buffer[4] = 'M';
        buffer[5] = 'D';
        buffer[6] = ':';

        ptr += 9;
        for (short i = 0;; i++) {
            char temp[3]      = {0};
            unsigned char tmp = '?', result = 0;

            temp[0] = *(ptr + i * 3);
            temp[1] = *(ptr + i * 3 + 1);

            for (unsigned char j = 0; j < 2; j++) {
                if ((temp[j] >= '0') && (temp[j] <= '9'))
                    tmp = temp[j] - '0';
                else if ((temp[j] >= 'A') && (temp[j] <= 'F'))
                    tmp = temp[j] - 'A' + 10;
                else if ((temp[j] >= 'a') && (temp[j] <= 'f'))
                    tmp = temp[j] - 'a' + 10;

                result = result * 16 + tmp;
            }

            if ((i + 7) < length) buffer[i + 7] = result;

            if (*(ptr + i * 3 + 3) == '\"' && *(ptr + i * 3 + 4) == '\r' &&
                *(ptr + i * 3 + 5) == '\n') {
                number = i + 1 + 7;
                break;
            }
        }
    }

    memset(_buffer, 0, BEFFER_LENGTH_MAX);

    return number;
}

bool LoRaWanClass::transferProprietaryPacket(char *buffer,
                                             unsigned char timeout) {
    unsigned char length = strlen(buffer);

    while (SerialLoRa.available()) SerialLoRa.read();

    sendCommand("AT+PMSG=\"");
    for (int i = 0; i < length; i++) SerialLoRa.write(buffer[i]);
    sendCommand("\"\r\n");

    memset(_buffer, 0, BEFFER_LENGTH_MAX);
    readBuffer(_buffer, BEFFER_LENGTH_MAX, timeout);
#if _DEBUG_SERIAL_
    SerialUSB.print(_buffer);
#endif
    if (strstr(_buffer, "+PMSG: Done")) return true;
    return false;
}

bool LoRaWanClass::transferProprietaryPacket(unsigned char *buffer,
                                             unsigned char length,
                                             unsigned char timeout) {
    char temp[3] = {0};

    while (SerialLoRa.available()) SerialLoRa.read();

    sendCommand("AT+PMSGHEX=\"");
    for (int i = 0; i < length; i++) {
        sprintf(temp, "%02x", buffer[i]);
        SerialLoRa.write(temp);
    }
    sendCommand("\"\r\n");

    memset(_buffer, 0, BEFFER_LENGTH_MAX);
    readBuffer(_buffer, BEFFER_LENGTH_MAX, timeout);
#if _DEBUG_SERIAL_
    SerialUSB.print(_buffer);
#endif
    if (strstr(_buffer, "+PMSGHEX: Done")) return true;
    return false;
}

void LoRaWanClass::setUnconfirmedMessageRepeatTime(unsigned char time) {
    char cmd[32];

    if (time > 15)
        time = 15;
    else if (time == 0)
        time = 1;

    memset(cmd, 0, 32);
    sprintf(cmd, "AT+REPT=%d\r\n", time);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setConfirmedMessageRetryTime(unsigned char time) {
    char cmd[32];

    if (time > 15)
        time = 15;
    else if (time == 0)
        time = 1;

    memset(cmd, 0, 32);
    sprintf(cmd, "AT+RETRY=%d\r\n", time);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::getReceiveWindowFirst(void) {
    sendCommand("AT+RXWIN1\r\n");
    loraDebugPrint(DEFAULT_DEBUGTIME);
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setReceiveWindowFirst(bool command) {
    if (command)
        sendCommand("AT+RXWIN1=ON\r\n");
    else
        sendCommand("AT+RXWIN1=OFF\r\n");
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}
void LoRaWanClass::setReceiveWindowFirst(unsigned char channel,
                                         float frequency) {
    char cmd[32];

    //    if(channel > 16) channel = 16;

    memset(cmd, 0, 32);
    if (frequency == 0)
        sprintf(cmd, "AT+RXWIN1=%d,0\r\n", channel);
    else
        sprintf(cmd, "AT+RXWIN1=%d,%d.%d\r\n", channel, (short)frequency,
                short(frequency * 10) % 10);
    sendCommand(cmd);
    SerialUSB.print(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME *
                   4);  // this can have a lot of data to dump
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setReceiveWindowSecond(float frequency,
                                          _data_rate_t dataRate) {
    char cmd[32];

    memset(cmd, 0, 32);
    sprintf(cmd, "AT+RXWIN2=%d.%d,%d\r\n", (short)frequency,
            short(frequency * 10) % 10, dataRate);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setReceiveWindowSecond(float frequency,
                                          _spreading_factor_t spreadingFactor,
                                          _band_width_t bandwidth) {
    char cmd[32];

    memset(cmd, 0, 32);
    sprintf(cmd, "AT+RXWIN2=%d.%d,%d,%d\r\n", (short)frequency,
            short(frequency * 10) % 10, spreadingFactor, bandwidth);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setReceiveWindowDelay(_window_delay_t command,
                                         unsigned short _delay) {
    char cmd[32];

    memset(cmd, 0, 32);
    if (command == RECEIVE_DELAY1)
        sprintf(cmd, "AT+DELAY=RX1,%d\r\n", _delay);
    else if (command == RECEIVE_DELAY2)
        sprintf(cmd, "AT+DELAY=RX2,%d\r\n", _delay);
    else if (command == JOIN_ACCEPT_DELAY1)
        sprintf(cmd, "AT+DELAY=JRX1,%d\r\n", _delay);
    else if (command == JOIN_ACCEPT_DELAY2)
        sprintf(cmd, "AT+DELAY=JRX2,%d\r\n", _delay);
    sendCommand(cmd);
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::setClassType(_class_type_t type) {
    if (type == CLASS_A)
        sendCommand("AT+CLASS=A\r\n");
    else if (type == CLASS_C)
        sendCommand("AT+CLASS=C\r\n");
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

//
// set the JOIN mode to either LWOTAA or LWABP
// does a half-hearted attempt to check the results
//
bool LoRaWanClass::setDeviceMode(_device_mode_t mode) {
    char buffer[kLOCAL_BUFF_MAX];
    int timeout = 1;

    if (mode == LWABP)
        sendCommand("AT+MODE=LWABP\r\n");
    else if (mode == LWOTAA)
        sendCommand("AT+MODE=LWOTAA\r\n");
    else
        return false;

    memset(buffer, 0, kLOCAL_BUFF_MAX);
    readBuffer(buffer, kLOCAL_BUFF_MAX - 1, timeout);

#if _DEBUG_SERIAL_
    SerialUSB.print(buffer);
//    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);

    return strstr(
        buffer, "+MODE:");  // if it works, response is of form "+MODE: LWOTTA"
}

//
//  JOIN with the application
//
//  setDeviceMode should have been called before this.
bool LoRaWanClass::setOTAAJoin(_otaa_join_cmd_t command,
                               unsigned char timeout) {
    // char *ptr;
    short count;
    bool joined = false;

    if (command == JOIN)
        sendCommand("AT+JOIN\r\n");
    else if (command == FORCE)
        sendCommand("AT+JOIN=FORCE\r\n");
    else {
        SerialUSB.print("Bad command to setOTAAJoin\n");
        return false;
    }

#if _DEBUG_SERIAL_
//    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    //    delay(DEFAULT_TIMEWAIT);

    while (true) {
        memset(_buffer, 0, BEFFER_LENGTH_MAX);
        count = readLine(_buffer, BEFFER_LENGTH_MAX, timeout);
        if (count == 0) continue;

            // !!! handle timeout
#if _DEBUG_SERIAL_
        SerialUSB.print(_buffer);
#endif
        if (strstr(_buffer, "+JOIN: Join failed")) continue;
        if (strstr(_buffer, "+JOIN: LoRaWAN modem is busy")) continue;
        if (strstr(_buffer, "+JOIN: NORMAL")) continue;
        if (strstr(_buffer, "+JOIN: FORCE")) continue;
        if (strstr(_buffer, "+JOIN: Start")) continue;
        if (strstr(_buffer, "+JOIN: Done")) break;
        if (strstr(_buffer, "+JOIN: No free channel")) break;
        if (strstr(_buffer, "+JOIN: Network joined")) {
            joined = true;
            continue;
        }
        if (strstr(_buffer, "+JOIN: NetID")) {
            joined = true;
            continue;
        }

        SerialUSB.print("Result didn't match anything I expected.\n");
    }

    SerialUSB.print("Done with Join\n");
    return joined;
}

void LoRaWanClass::setDeviceLowPower(void) {
    sendCommand("AT+LOWPOWER\r\n");
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

//
// Reset the LoRa module. Does not factory reset
//
void LoRaWanClass::setDeviceReset(void) {
    sendCommand("AT+RESET\r\n");
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

//
//  Factory reset the module.
//
void LoRaWanClass::setDeviceDefault(void) {
    sendCommand("AT+FDEFAULT=RISINGHF\r\n");
#if _DEBUG_SERIAL_
    loraDebugPrint(DEFAULT_DEBUGTIME);
#endif
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::initP2PMode(unsigned short frequency,
                               _spreading_factor_t spreadingFactor,
                               _band_width_t bandwidth,
                               unsigned char txPreamble,
                               unsigned char rxPreamble, short power) {
    char cmd[64] = {
        0,
    };
    sprintf(cmd, "AT+TEST=RFCFG,%d,%d,%d,%d,%d,%d\r\n", frequency,
            spreadingFactor, bandwidth, txPreamble, rxPreamble, power);

    sendCommand("AT+MODE=TEST\r\n");
    delay(DEFAULT_TIMEWAIT);
    sendCommand(cmd);
    delay(DEFAULT_TIMEWAIT);
    sendCommand("AT+TEST=RXLRPKT\r\n");
    delay(DEFAULT_TIMEWAIT);
}

void LoRaWanClass::transferPacketP2PMode(char *buffer) {
    unsigned char length = strlen(buffer);

    sendCommand("AT+TEST=TXLRSTR,\"");
    for (int i = 0; i < length; i++) SerialLoRa.write(buffer[i]);
    sendCommand("\"\r\n");
}

void LoRaWanClass::transferPacketP2PMode(unsigned char *buffer,
                                         unsigned char length) {
    char temp[3] = {0};

    sendCommand("AT+TEST=TXLRPKT,\"");
    for (int i = 0; i < length; i++) {
        sprintf(temp, "%02x", buffer[i]);
        SerialLoRa.write(temp);
    }
    sendCommand("\"\r\n");
}

short LoRaWanClass::receivePacketP2PMode(unsigned char *buffer, short length,
                                         short *rssi, unsigned char timeout) {
    char *ptr;
    short number;

    while (SerialLoRa.available()) SerialLoRa.read();
    memset(_buffer, 0, BEFFER_LENGTH_MAX);
    readBuffer(_buffer, BEFFER_LENGTH_MAX, timeout);

    ptr = strstr(_buffer, "LEN");
    if (ptr)
        number = atoi(ptr + 4);
    else
        number = 0;

    if (number <= 0) return 0;

    ptr = strstr(_buffer, "RSSI:");
    if (ptr)
        *rssi = atoi(ptr + 5);
    else
        *rssi = -255;

    ptr = strstr(_buffer, "RX \"");
    if (ptr) {
        ptr += 4;
        for (short i = 0; i < number; i++) {
            char temp[3]      = {0};
            unsigned char tmp = '?', result = 0;

            temp[0] = *(ptr + i * 2);
            temp[1] = *(ptr + i * 2 + 1);

            for (unsigned char j = 0; j < 2; j++) {
                if ((temp[j] >= '0') && (temp[j] <= '9'))
                    tmp = temp[j] - '0';
                else if ((temp[j] >= 'A') && (temp[j] <= 'F'))
                    tmp = temp[j] - 'A' + 10;
                else if ((temp[j] >= 'a') && (temp[j] <= 'f'))
                    tmp = temp[j] - 'a' + 10;

                result = result * 16 + tmp;
            }

            if (i < length) buffer[i] = result;
        }
    }

    memset(_buffer, 0, BEFFER_LENGTH_MAX);

    return number;
}

// short LoRaWanClass::getBatteryVoltage(void)
// {
//     int battery;

//     pinMode(CHARGE_STATUS_PIN, OUTPUT);
//     digitalWrite(CHARGE_STATUS_PIN, LOW);
//     delay(DEFAULT_TIMEWAIT);
//     battery = (analogRead(BATTERY_POWER_PIN) * 3300 * 11) >> 10;
//     pinMode(CHARGE_STATUS_PIN, INPUT);

//     return battery;
// }

// ??? I think this essentially connects the serial port to the LoRa module.
// @@@ typing a "~" will exit
//
void LoRaWanClass::loraDebug(void) {
    char c;

    while (true) {
        if (SerialUSB.available()) {
            c = SerialUSB.read();
            if (c == '~') return;
            SerialLoRa.write(c);
        }
        if (SerialLoRa.available()) SerialUSB.write(SerialLoRa.read());
    }
}

#if _DEBUG_SERIAL_
//
//  timeout is the total amount of time allowed for collecting data
//  Would it make more sense if it was the time w/o a character?
void LoRaWanClass::loraDebugPrint(unsigned int timeout) {
    unsigned long timerStart, timerEnd;
    char c;

    timerStart = millis();

    while (1) {
        while (SerialLoRa.available()) {
            SerialUSB.write(c = SerialLoRa.read());
            if (c == '\n')
                return;  // !!! This won't work for commands that return
                         // multiple lines.
            timerStart = millis();
        }

        timerEnd = millis();
        //        if(timerEnd - timerStart > 1000 * timeout)break;
        if (timerEnd - timerStart > timeout) break;
    }
}
#endif

void LoRaWanClass::debugPrint(const char *str) {
    SerialUSB.print(str);
}

void LoRaWanClass::sendCommand(const char *command) {
    SerialLoRa.print(command);
}

void LoRaWanClass::sendCommand(const __FlashStringHelper *command) {
    SerialLoRa.print(command);
}

short LoRaWanClass::readBuffer(char *buffer, short length,
                               unsigned char timeout) {
    short i = 0;
    unsigned long timerStart, timerEnd;

    timerStart = millis();

    while (1) {
        if (i < length) {
            while (SerialLoRa.available()) {
                char c      = SerialLoRa.read();
                buffer[i++] = c;
            }
        }

        timerEnd = millis();
        if (timerEnd - timerStart > 1000 * timeout) break;
    }

    return i;
}

short LoRaWanClass::readLine(char *buffer, short length,
                             unsigned char timeout) {
    short i = 0;
    unsigned long timerStart, timerEnd;
    char c = '\n';

    timerStart = millis();

    while (1) {
        if (i < length - 1) {
            while (SerialLoRa.available()) {
                c           = SerialLoRa.read();
                buffer[i++] = c;
                if (c == '\n') break;
                timerStart = millis();
            }
        }
        if (c == '\n') break;  // @@@ barf
        timerEnd = millis();
        if (timerEnd - timerStart > 1000 * timeout) break;
    }

    buffer[i] = 0;  // terminate the string
    return i;
}
short LoRaWanClass::waitForResponse(char *response, unsigned char timeout) {
    short len = strlen(response);
    short sum = 0;
    unsigned long timerStart, timerEnd;

    timerStart = millis();

    while (1) {
        if (SerialLoRa.available()) {
            char c = SerialLoRa.read();

            sum = (c == response[sum]) ? sum + 1 : 0;
            if (sum == len) break;
        }

        timerEnd = millis();
        if (timerEnd - timerStart > 1000 * timeout) return -1;
    }

    return 0;
}

short LoRaWanClass::sendCommandAndWaitForResponse(char *command, char *response,
                                                  unsigned char timeout) {
    sendCommand(command);

    return waitForResponse(response, timeout);
}

LoRaWanClass lora;
#endif