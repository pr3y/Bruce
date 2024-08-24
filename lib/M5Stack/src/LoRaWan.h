#if defined (CORE)
/*
  LoRaWAN.cpp for M5Stack (fork from
  https://github.com/toddkrein/OTAA-LoRaWAN-Seeed)

  2013 Copyright (c) Seeed Technology Inc.  All right reserved.
  Author: Wayne Weng
  Date: 2016-10-17
  add rgb backlight fucnction @ 2013-10-15

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

#ifndef _LORAWAN_H_
#define _LORAWAN_H_

#include <Arduino.h>

#define SerialUSB  Serial
#define SerialLoRa Serial2

#define _DEBUG_SERIAL_ 1
//#define DEFAULT_TIMEOUT     5 // second
#define DEFAULT_TIMEOUT 3  // second
#define DEFAULT_TIMEWAIT \
    200  // milliseconds to wait after issuing command via serial
#define DEFAULT_DEBUGTIME \
    500  // milliseconds to wait for a response after command

// #define BATTERY_POWER_PIN    A4
// #define CHARGE_STATUS_PIN    A5

#define BEFFER_LENGTH_MAX 256

#define MAC_COMMAND_FLAG "MACCMD:"
#define kLOCAL_BUFF_MAX  64

enum _class_type_t { CLASS_A = 0, CLASS_C };
enum _physical_type_t {
    UNINIT = -1,
    EU434,
    EU868,
    US915,
    US915HYBRID,
    AU915,
    AS923,
    CN470,
    KR920,
    CN470PREQUEL,
    STE920,
    UNDEF
};
enum _device_mode_t { LWABP = 0, LWOTAA, TEST };
enum _otaa_join_cmd_t { JOIN = 0, FORCE };
enum _window_delay_t {
    RECEIVE_DELAY1 = 0,
    RECEIVE_DELAY2,
    JOIN_ACCEPT_DELAY1,
    JOIN_ACCEPT_DELAY2
};
enum _band_width_t { BW125 = 125, BW250 = 250, BW500 = 500 };
enum _spreading_factor_t {
    SF12 = 12,
    SF11 = 11,
    SF10 = 10,
    SF9  = 9,
    SF8  = 8,
    SF7  = 7
};
enum _data_rate_t {
    DR0 = 0,
    DR1,
    DR2,
    DR3,
    DR4,
    DR5,
    DR6,
    DR7,
    DR8,
    DR9,
    DR10,
    DR11,
    DR12,
    DR13,
    DR14,
    DR15
};

/*****************************************************************
Type    DataRate    Configuration   BitRate| TxPower Configuration
EU434   0           SF12/125 kHz    250    | 0       10dBm
        1           SF11/125 kHz    440    | 1       7 dBm
        2           SF10/125 kHz    980    | 2       4 dBm
        3           SF9 /125 kHz    1760   | 3       1 dBm
        4           SF8 /125 kHz    3125   | 4       -2dBm
        5           SF7 /125 kHz    5470   | 5       -5dBm
        6           SF7 /250 kHz    11000  | 6:15    RFU
        7           FSK:50 kbps     50000  |
        8:15        RFU                    |
******************************************************************
Type    DataRate    Configuration   BitRate| TxPower Configuration
EU868   0           SF12/125 kHz    250    | 0       20dBm
        1           SF11/125 kHz    440    | 1       14dBm
        2           SF10/125 kHz    980    | 2       11dBm
        3           SF9 /125 kHz    1760   | 3       8 dBm
        4           SF8 /125 kHz    3125   | 4       5 dBm
        5           SF7 /125 kHz    5470   | 5       2 dBm
        6           SF7 /250 kHz    11000  | 6:15    RFU
        7           FSK:50 kbps     50000  |
        8:15        RFU                    |
******************************************************************
Type    DataRate    Configuration   BitRate| TxPower Configuration
US915   0           SF10/125 kHz    980    | 0       30dBm
        1           SF9 /125 kHz    1760   | 1       28dBm
        2           SF8 /125 kHz    3125   | 2       26dBm
        3           SF7 /125 kHz    5470   | 3       24dBm
        4           SF8 /500 kHz    12500  | 4       22dBm
        5:7         RFU                    | 5       20dBm
        8           SF12/500 kHz    980    | 6       18dBm
        9           SF11/500 kHz    1760   | 7       16dBm
        10          SF10/500 kHz    3900   | 8       14dBm
        11          SF9 /500 kHz    7000   | 9       12dBm
        12          SF8 /500 kHz    12500  | 10      10dBm
        13          SF7 /500 kHz    21900  | 11:15   RFU
        14:15       RFU                    |
*******************************************************************
Type    DataRate    Configuration   BitRate| TxPower Configuration
CN780   0           SF12/125 kHz    250    | 0       10dBm
        1           SF11/125 kHz    440    | 1       7 dBm
        2           SF10/125 kHz    980    | 2       4 dBm
        3           SF9 /125 kHz    1760   | 3       1 dBm
        4           SF8 /125 kHz    3125   | 4       -2dBm
        5           SF7 /125 kHz    5470   | 5       -5dBm
        6           SF7 /250 kHz    11000  | 6:15    RFU
        7           FSK:50 kbps     50000  |
        8:15        RFU                    |
******************************************************************/

class LoRaWanClass {
   public:
    LoRaWanClass(void);

    /**
     *  \brief Initialize the conmunication interface
     *
     *  \return Return null
     */
    void init(void);

    /**
     *  \brief Read the version from device
     *
     *  \param [in] *buffer The output data cache
     *  \param [in] length The length of data cache
     *  \param [in] timeout The over time of read
     *
     *  \return Return null.
     */
    void getVersion(char *buffer, short length,
                    unsigned char timeout = DEFAULT_TIMEOUT);

    /**
     *  \brief Read the ID from device
     *
     *  \param [in] *buffer The output data cache
     *  \param [in] length The length of data cache
     *  \param [in] timeout The over time of read
     *
     *  \return Return null.
     */
    void getId(char *buffer, short length,
               unsigned char timeout = DEFAULT_TIMEOUT);

    /**
     *  \brief Set the ID
     *
     *  \param [in] *DevAddr The end-device address
     *  \param [in] *DevEUI The end-device identifier
     *  \param [in] *AppEUI The application identifier
     *
     *  \return Return null.
     */
    void setId(char *DevAddr, char *DevEUI, char *AppEUI);

    /**
     *  \brief Set the key
     *
     *  \param [in] *NwkSKey The network session key
     *  \param [in] *AppSKey The application session key
     *  \param [in] *AppKey The Application key
     *
     *  \return Return null.
     */
    void setKey(char *NwkSKey, char *AppSKey, char *AppKey);

    /**
     *  \brief Set the data rate
     *
     *  \param [in] dataRate The date rate of encoding
     *  \param [in] physicalType The type of ISM
     *
     *  \return Return null.
     */
    bool setDataRate(_data_rate_t dataRate         = DR0,
                     _physical_type_t physicalType = EU434);

    /**
     *  \brief ON/OFF adaptive data rate mode
     *
     *  \param [in] command The date rate of encoding
     *
     *  \return Return null.
     */
    void setAdaptiveDataRate(bool command);

    /**
     *  \brief Set the output power
     *
     *  \param [in] power The output power value
     *
     *  \return Return null.
     */
    void setPower(short power);

    /**
     *  \brief Set the port number
     *
     *  \param [in] port The port number, range from 1 to 255
     *
     *  \return Return null.
     */
    void setPort(unsigned char port);

    /**
     *  \brief Set the channel parameter
     *
     *  \param [in] channel The channel number, range from 0 to 15
     *  \param [in] frequency The frequency value
     *
     *  \return Return null.
     */
    void getChannel(void);
    void setChannel(unsigned char channel, float frequency);
    /**
     *  \brief Set the channel parameter
     *
     *  \param [in] channel The channel number, range from 0 to 15
     *  \param [in] frequency The frequency value. Set frequecy zero to disable
     * one channel \param [in] dataRata The date rate of channel
     *
     *  \return Return null.
     */
    void setChannel(unsigned char channel, float frequency,
                    _data_rate_t dataRata);
    /**
     *  \brief Set the channel parameter
     *
     *  \param [in] channel The channel number, range from 0 to 15
     *  \param [in] frequency The frequency value
     *  \param [in] dataRataMin The minimum date rate of channel
     *  \param [in] dataRataMax The maximum date rate of channel
     *
     *  \return Return null.
     */
    void setChannel(unsigned char channel, float frequency,
                    _data_rate_t dataRataMin, _data_rate_t dataRataMax);

    /**
     *  \brief Transfer the data
     *
     *  \param [in] *buffer The transfer data cache
     *  \param [in] timeout The over time of transfer
     *
     *  \return Return bool. Ture : transfer done, false : transfer failed
     */
    bool transferPacket(char *buffer, unsigned char timeout = DEFAULT_TIMEOUT);
    /**
     *  \brief Transfer the data
     *
     *  \param [in] *buffer The transfer data cache
     *  \param [in] length The length of data cache
     *  \param [in] timeout The over time of transfer
     *
     *  \return Return bool. Ture : transfer done, false : transfer failed
     */
    bool transferPacket(unsigned char *buffer, unsigned char length,
                        unsigned char timeout = DEFAULT_TIMEOUT);
    /**
     *  \brief Transfer the packet data
     *
     *  \param [in] *buffer The transfer data cache
     *  \param [in] timeout The over time of transfer
     *
     *  \return Return bool. Ture : Confirmed ACK, false : Confirmed NOT ACK
     */
    bool transferPacketWithConfirmed(char *buffer,
                                     unsigned char timeout = DEFAULT_TIMEOUT);
    /**
     *  \brief Transfer the data
     *
     *  \param [in] *buffer The transfer data cache
     *  \param [in] length The length of data cache
     *  \param [in] timeout The over time of transfer
     *
     *  \return Return bool. Ture : Confirmed ACK, false : Confirmed NOT ACK
     */
    bool transferPacketWithConfirmed(unsigned char *buffer,
                                     unsigned char length,
                                     unsigned char timeout = DEFAULT_TIMEOUT);

    /**
     *  \brief Receive the data
     *
     *  \param [in] *buffer The receive data cache
     *  \param [in] length The length of data cache
     *  \param [in] *rssi The RSSI cache
     *
     *  \return Return Receive data number
     */
    short receivePacket(char *buffer, short length, short *rssi);

    /**
     *  \brief Transfer the proprietary data
     *
     *  \param [in] *buffer The transfer data cache
     *  \param [in] timeout The over time of transfer
     *
     *  \return Return bool. Ture : transfer done, false : transfer failed
     */
    bool transferProprietaryPacket(char *buffer,
                                   unsigned char timeout = DEFAULT_TIMEOUT);
    /**
     *  \brief Transfer the proprietary data
     *
     *  \param [in] *buffer The transfer data cache
     *  \param [in] length The length of data cache
     *  \param [in] timeout The over time of transfer
     *
     *  \return Return bool. Ture : transfer done, false : transfer failed
     */
    bool transferProprietaryPacket(unsigned char *buffer, unsigned char length,
                                   unsigned char timeout = DEFAULT_TIMEOUT);

    /**
     *  \brief Set device mode
     *
     *  \param [in] mode The mode of device
     *
     *  \return Return null
     */
    bool setDeviceMode(_device_mode_t mode);

    /**
     *  \brief Set device join a network
     *
     *  \param [in] command The type of join
     *  \param [in] timeout The over time of join
     *
     *  \return Return bool. True : join OK, false : join NOT OK
     */
    bool setOTAAJoin(_otaa_join_cmd_t command,
                     unsigned char timeout = DEFAULT_TIMEOUT);

    /**
     *  \brief Set message unconfirmed repeat time
     *
     *  \param [in] time The repeat time, range from 1 to 15
     *
     *  \return Return null
     */
    void setUnconfirmedMessageRepeatTime(unsigned char time);

    /**
     *  \brief Set message retry times time
     *
     *  \param [in] time The retry time, range from 0 to 254
     *
     *  \return Return null
     */
    void setConfirmedMessageRetryTime(unsigned char time);

    /**
     *  \brief ON/OFF receice window 1
     *
     *  \param [in] command The true : ON, false OFF
     *
     *  \return Return null
     */
    void getReceiveWindowFirst(void);
    void setReceiveWindowFirst(bool command);
    /**
     *  \brief Set receice window 1 channel mapping
     *
     *  \param [in] channel The channel number, range from 0 to 71
     *  \param [in] frequency The frequency value of channel
     *
     *  \return Return null
     */
    void setReceiveWindowFirst(unsigned char channel, float frequency);

    /**
     *  \brief Set receice window 2 channel mapping
     *
     *  \param [in] frequency The frequency value of channel
     *  \param [in] dataRate The date rate value
     *
     *  \return Return null
     */
    void setReceiveWindowSecond(float frequency, _data_rate_t dataRate);
    /**
     *  \brief Set receice window 2 channel mapping
     *
     *  \param [in] frequency The frequency value of channel
     *  \param [in] spreadingFactor The spreading factor value
     *  \param [in] bandwidth The band width value
     *
     *  \return Return null
     */
    void setReceiveWindowSecond(float frequency,
                                _spreading_factor_t spreadingFactor,
                                _band_width_t bandwidth);

    /**
     *  \brief Set receice window delay
     *
     *  \param [in] command The delay type
     *  \param [in] _delay The delay value(millisecond)
     *
     *  \return Return null
     */
    void setReceiveWindowDelay(_window_delay_t command, unsigned short _delay);

    /**
     *  \brief Set LoRaWAN class type
     *
     *  \param [in] type The class type
     *
     *  \return Return null
     */
    void setClassType(_class_type_t type);

    /**
     *  \brief Set device into low power mode
     *
     *  \return Return null
     */
    void setDeviceLowPower(void);

    /**
     *  \brief Reset device
     *
     *  \return Return null
     */
    void setDeviceReset(void);

    /**
     *  \brief Setup device default
     *
     *  \return Return null
     */
    void setDeviceDefault(void);

    /**
     *  \brief Initialize device into P2P mode
     *
     *  \param [in] frequency The ISM frequency value
     *  \param [in] spreadingFactor The spreading factor value
     *  \param [in] bandwidth The band width value
     *  \param [in] txPreamble The transfer packet preamble number
     *  \param [in] rxPreamble The receive packet preamble number
     *  \param [in] power The output power
     *
     *  \return Return null
     */
    void initP2PMode(unsigned short frequency            = 433,
                     _spreading_factor_t spreadingFactor = SF12,
                     _band_width_t bandwidth             = BW125,
                     unsigned char txPreamble = 8, unsigned char rxPreamble = 8,
                     short power = 20);
    /**
     *  \brief Transfer the data
     *
     *  \param [in] *buffer The transfer data cache
     *
     *  \return Return bool. Ture : transfer done, false : transfer failed
     */
    void transferPacketP2PMode(char *buffer);
    /**
     *  \brief Transfer the data
     *
     *  \param [in] *buffer The transfer data cache
     *  \param [in] length The length of data cache
     *
     *  \return Return bool. Ture : transfer done, false : transfer failed
     */
    void transferPacketP2PMode(unsigned char *buffer, unsigned char length);
    /**
     *  \brief Receive the data
     *
     *  \param [in] *buffer The receive data cache
     *  \param [in] length The length of data cache
     *  \param [in] *rssi The RSSI cache
     *  \param [in] timeout The over time of receive
     *
     *  \return Return Receive data number
     */
    short receivePacketP2PMode(unsigned char *buffer, short length, short *rssi,
                               unsigned char timeout = DEFAULT_TIMEOUT);

    /**
     *  \brief LoRaWAN raw data
     *
     *  \return Return null
     */
    void loraDebug(void);
#if _DEBUG_SERIAL_
    void loraDebugPrint(unsigned int timeout);
#endif
    void debugPrint(const char *str);
    /**
     *  \brief Read battery voltage
     *
     *  \return Return battery voltage
     */
    // short getBatteryVoltage(void);

   private:
    void sendCommand(const char *command);
    void sendCommand(const __FlashStringHelper *command);
    short readBuffer(char *buffer, short length,
                     unsigned char timeout = DEFAULT_TIMEOUT);
    short readLine(char *buffer, short length,
                   unsigned char timeout = DEFAULT_TIMEOUT);
    short waitForResponse(char *response,
                          unsigned char timeout = DEFAULT_TIMEOUT);
    short sendCommandAndWaitForResponse(
        char *command, char *response, unsigned char timeout = DEFAULT_TIMEOUT);

    char _buffer[256];
    short debug;
    _physical_type_t myType;
};

extern LoRaWanClass lora;

#endif
#endif