#include "fm.h"

// #define _BV(n) (1 << n)
// #define FMSTATION 10230      // 10230 == 102.30 MHz
#define RESETPIN 0

bool auto_scan = false;
uint16_t fm_station = 10230; // Default set to 102.30 MHz
Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

uint16_t scan_fm() {
  uint16_t f = 8750;
  uint16_t min_noise;
  uint16_t freq_candidate = f;

  // Check for first noise level
  radio.readTuneMeasure(f);
  radio.readTuneStatus();
  min_noise = radio.currNoiseLevel;

  tft.fillScreen(BGCOLOR);
  // displayRedStripe("Scanning...", TFT_WHITE, FGCOLOR);
  for (f=8750; f<10800; f+=10) {
    Serial.print("Measuring "); Serial.print(f); Serial.print("...");
    tft.print("Measuring "); tft.print(f); tft.print("...");
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

  return freq_candidate;
}

void fm_banner() {
  tft.fillScreen(BGCOLOR);
  tft.setCursor(10, 10);
  tft.drawCentreString("Running Bruce Radio", WIDTH/2, 10, SMOOTH_FONT);
  delay(500);
}

void set_auto_scan(bool new_value) {
  auto_scan = new_value;
}

void set_frq(uint16_t frq) {
  fm_station = frq;
}

void fm_options() {
  char f_str[5];
  // Choose between scan for best freq or select freq
  displayRedStripe("Choose frequency", TFT_WHITE, FGCOLOR);
  delay(1000);

  options = { };
  options.push_back({"Auto",       [=]() { set_auto_scan(true); }});
  for(uint16_t f=8750; f<10800; f+=10){
    sprintf(f_str, "%d MHz", f);
    options.push_back({f_str,      [=]() { set_frq(true); }});
  }
  options.push_back({"Main Menu",  [=]() { backToMenu(); }});
  delay(200);
  loopOptions(options);

  if (auto_scan == true) {
    fm_station = scan_fm();
  }
}

void fm_live_run() {
  char brd_str[5];
  fm_banner();

  if (fm_setup(fm_station)) {
    fm_options();
    if (!returnToMenu) {
      sprintf(brd_str, "Broadcast %d MHz", fm_station);
      displayRedStripe(brd_str, TFT_WHITE, FGCOLOR);
      while(!checkEscPress() && !checkSelPress()) {
          fm_loop();
      }
    }
  }

  fm_stop();
}

void fm_zic_run() {
  fm_banner();
  if (fm_setup(fm_station)) {
    fm_options();
  }
  fm_stop();
}

void fm_ta_run() {
  // Set Info Traffic
  fm_station = 10770;
  fm_banner();
  fm_stop();
}

bool fm_setup(uint16_t freq) {
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

  Serial.print("\nSet TX power");
  tft.print("\nSet TX power");
  radio.setTXpower(115);  // dBuV, 88-115 max

  Serial.print("\nTuning into ");
  Serial.print(freq/100);
  Serial.print('.');
  Serial.println(freq % 100);
  tft.print('.');
  tft.print("\nTuning into ");
  tft.print(freq/100);
  tft.println(freq % 100);
  radio.tuneFM(freq); // 102.3 mhz

  // This will tell you the status in case you want to read it from the chip
  radio.readTuneStatus();
  Serial.print("\tCurr freq: ");
  Serial.println(radio.currFreq);
  tft.print("\tCurr freq: ");
  tft.println(radio.currFreq);
  Serial.print("\tCurr freqdBuV:");
  Serial.println(radio.currdBuV);
  tft.print("\tCurr freqdBuV:");
  tft.println(radio.currdBuV);
  Serial.print("\tCurr ANTcap:");
  Serial.println(radio.currAntCap);
  tft.print("\tCurr ANTcap:");
  tft.println(radio.currAntCap);

  // begin the RDS/RDBS transmission
  radio.beginRDS();
  radio.setRDSstation("BruceRadio");
  radio.setRDSbuffer( "Pwned by Bruce Radio!");

  Serial.println("RDS on!");
  tft.println("RDS on!");
  // radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output

  return true;
}

void fm_stop() {
  // Stop radio
  radio.reset();
}

void fm_loop() {
  radio.readASQ();
  /*Serial.print("\tCurr ASQ: 0x");
  Serial.println(radio.currASQ, HEX);
  tft.print("\tCurr ASQ: 0x");
  tft.println(radio.currASQ, HEX);
  Serial.print("\tCurr InLevel:");
  Serial.println(radio.currInLevel);
  tft.print("\tCurr InLevel:");
  tft.println(radio.currInLevel);*/
  // toggle GPO1 and GPO2
  // radio.setGPIO(_BV(1));
  // delay(500);
  // radio.setGPIO(_BV(2));
  delay(500); // Instead of 500
}
