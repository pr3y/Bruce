/**
 * @file wardriving.h
 * @author IncursioHack - https://github.com/IncursioHack
 * @brief WiFi Wardriving
 * @version 0.2
 * @note Updated: 2024-08-28 by Rennan Cockles (https://github.com/rennancockles)
 */

#ifndef WAR_DRIVING_H
#define WAR_DRIVING_H

#include "core/globals.h"
#include <TinyGPS++.h>
#include <set>
#include <esp_wifi_types.h>


class Wardriving {
public:
    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    Wardriving();
    ~Wardriving();

    /////////////////////////////////////////////////////////////////////////////////////
    // Life Cycle
    /////////////////////////////////////////////////////////////////////////////////////
    void setup();
    void loop();

private:
    bool date_time_updated = false;
    bool initial_position_set = false;
    double cur_lat;
    double cur_lng;
    double distance = 0;
    String filename = "";
    TinyGPSPlus gps;
    HardwareSerial GPSserial = HardwareSerial(2);     // Uses UART2 for GPS
    std::set<String> registeredMACs; // Store and track registered MAC
    int wifiNetworkCount = 0;        // Counter fo wifi networks

    /////////////////////////////////////////////////////////////////////////////////////
    // Setup
    /////////////////////////////////////////////////////////////////////////////////////
    void begin_wifi(void);
    bool begin_gps(void);
    void end(void);

    /////////////////////////////////////////////////////////////////////////////////////
    // Display functions
    /////////////////////////////////////////////////////////////////////////////////////
    void display_banner(void);
    void dump_gps_data(void);

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    void set_position(void);
    void scan_networks(void);
    String auth_mode_to_string(wifi_auth_mode_t authMode);
    void append_to_file(int network_amount);
    void create_filename(void);
};

#endif // WAR_DRIVING_H