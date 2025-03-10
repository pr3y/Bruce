/**
 * @file wardriving.cpp
 * @author IncursioHack - https://github.com/IncursioHack
 * @brief WiFi Wardriving
 * @version 0.2
 * @note Updated: 2024-08-28 by Rennan Cockles (https://github.com/rennancockles)
 */


#include "wardriving.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/wifi_common.h"
#include "core/sd_functions.h"

#define MAX_WAIT 5000
#define CURRENT_YEAR 2024


Wardriving::Wardriving() {
    setup();
}

Wardriving::~Wardriving() {
    if (gpsConnected) end();
}

void Wardriving::setup() {
    display_banner();
    padprintln("Initializing...");

    begin_wifi();
    if (!begin_gps()) return;

    delay(500);
    return loop();
}

void Wardriving::begin_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
}

bool Wardriving::begin_gps() {
    GPSserial.begin(bruceConfig.gpsBaudrate, SERIAL_8N1, SERIAL_RX, SERIAL_TX);

    int count = 0;
    padprintln("Waiting for GPS data");
    while(GPSserial.available() <= 0) {
        if(check(EscPress)) {
            end();
            return false;
        }
        displayTextLine("Waiting GPS: " + String(count)+ "s");
        count++;
        delay(1000);
    }

    gpsConnected = true;
    return true;
}

void Wardriving::end() {
    wifiDisconnect();

    GPSserial.end();

    returnToMenu = true;
    gpsConnected = false;

    delay(500);
}

void Wardriving::loop() {
    int count = 0;
    returnToMenu = false;
    while(1) {
        display_banner();

        if (check(EscPress) || returnToMenu) return end();

        if (GPSserial.available() > 0) {
            count = 0;
            while (GPSserial.available() > 0) gps.encode(GPSserial.read());

            if (gps.location.isUpdated()) {
                padprintln("GPS location updated");
                set_position();
                scan_networks();
            } else {
                padprintln("GPS location not updated");
                dump_gps_data();

                if (filename == "" && gps.date.year() >= CURRENT_YEAR && gps.date.year() < CURRENT_YEAR+5)
                    create_filename();
            }
        } else {
            if (count > 5) {
                displayError("GPS not Found!");
                return end();
            }
            padprintln("No GPS data available");
            count++;
        }

        int tmp = millis();
        while(millis()-tmp < MAX_WAIT && !gps.location.isUpdated()){
            if (check(EscPress) || returnToMenu) return end();
        }
    }
}

void Wardriving::set_position() {
    double lat = gps.location.lat();
    double lng = gps.location.lng();

    if (initial_position_set) distance += gps.distanceBetween(cur_lat, cur_lng, lat, lng);
    else initial_position_set = true;

    cur_lat = lat;
    cur_lng = lng;
}

void Wardriving::display_banner() {
    drawMainBorderWithTitle("Wardriving");
    padprintln("");

    if (wifiNetworkCount > 0){
        padprintln("File: " + filename.substring(0, filename.length()-4), 2);
        padprintln("Unique Networks Found: " + String(wifiNetworkCount), 2);
        padprintf(2, "Distance: %.2fkm\n", distance / 1000);
    }

    padprintln("");
}

void Wardriving::dump_gps_data() {
    if (!date_time_updated && (!gps.date.isUpdated() || !gps.time.isUpdated())) {
        padprintln("Waiting for valid GPS data");
        return;
    }
    date_time_updated = true;
    padprintf(2, "Date: %02d-%02d-%02d\n", gps.date.year(), gps.date.month(), gps.date.day());
    padprintf(2, "Time: %02d:%02d:%02d\n", gps.time.hour(), gps.time.minute(), gps.time.second());
    padprintf(2, "Sat:  %d\n", gps.satellites.value());
    padprintf(2, "HDOP: %.2f\n", gps.hdop.hdop());
}

String Wardriving::auth_mode_to_string(wifi_auth_mode_t authMode) {
  switch (authMode) {
    case WIFI_AUTH_OPEN: return "OPEN";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA_PSK";
    case WIFI_AUTH_WPA2_PSK: return "WPA2_PSK";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA_WPA2_PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2_ENTERPRISE";
    case WIFI_AUTH_WPA3_PSK: return "WPA3_PSK";
    case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2_WPA3_PSK";
    case WIFI_AUTH_WAPI_PSK: return "WAPI_PSK";
    default: return "UNKNOWN";
  }
}

void Wardriving::scan_networks() {
    wifiConnected = true;

    int network_amount = WiFi.scanNetworks();
    if (network_amount == 0) {
        padprintln("No Wi-Fi networks found", 2);
        return;
    }

    padprintf(2, "Coord: %.6f, %.6f\n", gps.location.lat(), gps.location.lng());
    padprintln("Networks Found: " + String(network_amount), 2);

    return append_to_file(network_amount);
}

void Wardriving::create_filename() {
    char timestamp[20];
    sprintf(
        timestamp,
        "%02d%02d%02d_%02d%02d%02d",
        gps.date.year() % 100,
        gps.date.month() % 100,
        gps.date.day() % 100,
        gps.time.hour() % 100,
        gps.time.minute() % 100,
        gps.time.second() % 100
    );
    filename = String(timestamp) + "_wardriving.csv";
}

void Wardriving::append_to_file(int network_amount) {
    FS *fs;
    if(!getFsStorage(fs)) {
        padprintln("Storage setup error");
        returnToMenu = true;
        return;
    }

    if (filename == "") create_filename();

    if (!(*fs).exists("/BruceWardriving")) (*fs).mkdir("/BruceWardriving");

    bool is_new_file = false;
    if(!(*fs).exists("/BruceWardriving/"+filename)) is_new_file = true;
    File file = (*fs).open("/BruceWardriving/"+filename, is_new_file ? FILE_WRITE : FILE_APPEND);

    if (!file) {
        padprintln("Failed to open file for writing");
        returnToMenu = true;
        return;
    }

    if (is_new_file) {
        file.println("WigleWifi-1.6,appRelease=v"+String(BRUCE_VERSION)+",model=M5Stack GPS Unit,release=v"+String(BRUCE_VERSION)+",device=ESP32 M5Stack,display=SPI TFT,board=ESP32 M5Stack,brand=Bruce,star=Sol,body=4,subBody=1");
        file.println("MAC,SSID,AuthMode,FirstSeen,Channel,Frequency,RSSI,CurrentLatitude,CurrentLongitude,AltitudeMeters,AccuracyMeters,RCOIs,MfgrId,Type");
    }

    for (int i = 0; i < network_amount; i++) {
        String macAddress = WiFi.BSSIDstr(i);

        // Check if MAC was already found in this session
        if (registeredMACs.find(macAddress) == registeredMACs.end()) {
            registeredMACs.insert(macAddress); // Adds MAC to file
            int32_t channel = WiFi.channel(i);

            char buffer[512];
            snprintf(buffer, sizeof(buffer), "%s,\"%s\",[%s],%04d-%02d-%02d %02d:%02d:%02d,%d,%d,%d,%f,%f,%f,%f,,,WIFI\n",
                macAddress.c_str(),
                WiFi.SSID(i).c_str(),
                auth_mode_to_string(WiFi.encryptionType(i)).c_str(),
                gps.date.year(), gps.date.month(), gps.date.day(),
                gps.time.hour(), gps.time.minute(), gps.time.second(),
                channel,
                channel != 14 ? 2407 + (channel * 5) : 2484,
                WiFi.RSSI(i),
                gps.location.lat(),
                gps.location.lng(),
                gps.altitude.meters(),
                gps.hdop.hdop() * 1.0
            );
            file.print(buffer);

            wifiNetworkCount++;
        }
    }

    file.close();
}
