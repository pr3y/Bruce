#include "fm.h"

// #define _BV(n) (1 << n)
// #define FMSTATION 10230      // 10230 == 102.30 MHz
#define RESETPIN 0

bool auto_scan = false;
uint16_t fm_station = 10230; // Default set to 102.30 MHz
Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

void set_auto_scan(bool new_value) {
  auto_scan = new_value;
}

void set_frq(uint16_t frq) {
  fm_station = frq;
}

void fm_banner() {
  tft.fillScreen(BGCOLOR);
  tft.setCursor(10, 10);
  tft.drawCentreString("~== Bruce Radio ==~", WIDTH/2, 10, SMOOTH_FONT);
  delay(500);
}

uint16_t scan_fm() {
  char display_freq[10];
  uint16_t f = 8750;
  uint16_t min_noise;
  uint16_t freq_candidate = f;

  // Check for first noise level
  radio.readTuneMeasure(f);
  radio.readTuneStatus();
  min_noise = radio.currNoiseLevel;

  tft.fillScreen(BGCOLOR);
  displayRedStripe("Scanning...", TFT_WHITE, FGCOLOR);
  for (f=8750; f<10800; f+=10) {
    // Serial.print("Measuring "); Serial.print(f); Serial.print("...");
    // tft.print("Measuring "); tft.print(f); tft.print("...");
    radio.readTuneMeasure(f);
    radio.readTuneStatus();
    Serial.println(radio.currNoiseLevel);
    tft.println(radio.currNoiseLevel);

    // Set best freq candidate
    if (radio.currNoiseLevel < min_noise) {
      min_noise = radio.currNoiseLevel;
      freq_candidate = f;
    }
  }

  sprintf(display_freq, "Found %d Mhz", freq_candidate);
  tft.fillScreen(BGCOLOR);
  while(!checkEscPress() && !checkSelPress()) {
    displayRedStripe(display_freq, TFT_WHITE, FGCOLOR);
    delay(100);
  }

  return freq_candidate;
}

// Choose between 92.0 - 92.1 - 92.2 - 92.3 etc.
void fm_options_frq(uint16_t f_min, bool reserved) {
  char f_str[5];
  uint16_t f_max;
  // Choose between scan for best freq or select freq
  displayRedStripe("Choose frequency", TFT_WHITE, FGCOLOR);
  delay(1000);

  // Handle min / max frequency
  if (reserved) {
    f_min = 7600;
    f_max = 8750;
  }
  else if (f_min < 90) {
    f_min = 8750;
    f_max = 9000;
  }
  else if (f_min < 100) {
    f_min = 9000;
    f_max = 10000;
  }
  else {
    f_min = 10000;
    f_max = 10810;
  }

  options = { };
  for(uint16_t f=f_min; f<f_max; f+=10){
    sprintf(f_str, "%d MHz", f);
    options.push_back({f_str,      [=]() { set_frq(f); }});
  }
  options.push_back({"Main Menu",  [=]() { backToMenu(); }});
  delay(200);
  loopOptions(options);

  if (auto_scan == true) {
    fm_station = scan_fm();
  }
}

// Choose between 91 - 92 - 93 etc.
void fm_options_digit(uint16_t f_min, bool reserved) {
  char f_str[5];
  uint16_t f_max;
  // Choose between scan for best freq or select freq
  displayRedStripe("Choose digit", TFT_WHITE, FGCOLOR);
  delay(1000);

  // Handle min / max frequency
  if (reserved) {
    f_min = 76;
    f_max = 88;
  }
  else if (f_min < 90) {
    f_min = 87;
    f_max = 90;
  }
  else if (f_min >= 100) {
    f_max = 108;
  }
  else {
    f_max = f_min + 10;
  }

  options = { };
  for(uint16_t f=f_min; f<f_max; f+=1){
    sprintf(f_str, "%d MHz", f);
    options.push_back({f_str,      [=]() { fm_options_frq(f, reserved); }});
  }
  options.push_back({"Main Menu",  [=]() { backToMenu(); }});
  delay(200);
  loopOptions(options);

  if (auto_scan == true) {
    fm_station = scan_fm();
  }
}

// Choose between 80 - 90 - 100
void fm_options(uint16_t f_min, uint16_t f_max, bool reserved) {
  char f_str[5];
  // Choose between scan for best freq or select freq
  displayRedStripe("Choose tens", TFT_WHITE, FGCOLOR);
  delay(1000);

  options = { };
  options.push_back({"Auto",       [=]() { set_auto_scan(true); }});
  for(uint16_t f=f_min; f<f_max; f+=10){
    sprintf(f_str, "%d MHz", f);
    options.push_back({f_str,      [=]() { fm_options_digit(f, reserved); }});
  }
  options.push_back({"Main Menu",  [=]() { backToMenu(); }});
  delay(200);
  loopOptions(options);

  if (auto_scan == true) {
    fm_station = scan_fm();
  }
}

void fm_live_run(bool reserved) {
  uint16_t f_min = 80;
  uint16_t f_max = 110;
  fm_banner();

  if (reserved) {
    f_min = 70;
    f_max = 90;
  }

  // Display choose frequency menu
  fm_options(f_min, f_max, reserved);

  // Run radio broadcast
  if (fm_setup(fm_station)) {
    fm_banner();
    if (!returnToMenu) {
      tft.print("Broadcast ");
      tft.print(fm_station);
      tft.println(" MHz");
      while(!checkEscPress() && !checkSelPress()) {
        delay(100);
      }
    }
  }

  // Stop radio before exit
  fm_stop();
}

void fm_ta_run() {
  // Set Info Traffic
  fm_station = 10770;
  fm_banner();

  // Run radio broadcast
  fm_setup(true);
  while(!checkEscPress() && !checkSelPress()) {
    delay(100);
  }

  fm_stop();
}

bool fm_setup(bool traffic_alert) {
  tft.setCursor(10, 40);
  Serial.println("Setup Si4713");
  tft.println("Setup Si4713");
  delay(1000);

  if (!radio.begin()) { // begin with address 0x63 (CS high default)
    tft.fillScreen(BGCOLOR);
    Serial.println("Cannot find radio");
    displayRedStripe("Cannot find radio", TFT_WHITE, FGCOLOR);
    while(!checkEscPress() && !checkSelPress()) {
      delay(100);
    }
    return false;
  }

  Serial.print("\nSet TX power to 115 (max)");
  tft.print("\nSet TX power 115 (max)");
  radio.setTXpower(115);  // dBuV, 88-115 max

  Serial.print("\nTuning into ");
  Serial.print(fm_station/100);
  Serial.print('.');
  Serial.println(fm_station % 100);
  tft.print("\nTuning into ");
  tft.print(fm_station/100);
  tft.print('.');
  tft.println(fm_station % 100);
  radio.tuneFM(fm_station); // 102.3 mhz

  // Begin the RDS/RDBS transmission
  radio.beginRDS();
  if (traffic_alert) {
    radio.setRDSstation("BruceTraffic");
    radio.setRDSbuffer("Traffic Info");
  }
  else {
    radio.setRDSstation("BruceRadio");
    radio.setRDSbuffer("Pwned by Bruce Radio!");
  }

  Serial.println("RDS on!");
  tft.println("RDS on!");

  if (traffic_alert) {
    radio.setProperty(SI4713_PROP_TX_RDS_PS_MISC, 0x1018);
  }

  return true;
}

void fm_stop() {
  // Stop radio
  radio.reset();
}

