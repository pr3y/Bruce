/**
 * @file wardriving.h
 * @author IncursioHack - https://github.com/IncursioHack
 * @brief WiFi Wardriving
 * @version 0.2
 * @note Updated: 2024-08-28 by Rennan Cockles (https://github.com/rennancockles)
 */

#ifndef __WAR_DRIVING_H__
#define __WAR_DRIVING_H__

#include <TinyGPS++.h>
#include <esp_wifi_types.h>
#include <globals.h>
#include <map>
#include <set>

struct MACLocationData {
    double lat;
    double lng;
};

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
    std::map<String, MACLocationData> registeredMACs; // Store MAC with last recorded location
    int wifiNetworkCount = 0;                         // Counter for wifi networks
    bool rxPinReleased = false;
    bool enableTriangulation = true;        // Enable multiple records per MAC for triangulation
    double triangulationMinDistance = 10.0; // Minimum distance (meters) to record same MAC again

    /////////////////////////////////////////////////////////////////////////////////////
    // Setup
    /////////////////////////////////////////////////////////////////////////////////////
    void show_config_menu(void);
    void begin_wifi(void);
    bool begin_gps(void);
    void end(void);
    void releasePins(void);
    void restorePins(void);

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
