#include "fm.h"

// #define _BV(n) (1 << n)
#define RESETPIN 0
#define FMSTATION 10290      // 10230 == 102.30 MHz

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

void fm_run() {
  tft.fillScreen(BGCOLOR);
  tft.setCursor(10, 10);
  tft.println("Running Bruce Radio");
  delay(500);
  fm_setup();
  while(!checkEscPress() && !checkSelPress()) {
      fm_loop();
      delay(10);
  }
}

void fm_setup() {
  Serial.println("Bruce Radio - Si4713");
  tft.println("Bruce Radio - Si4713");

  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    tft.println("Couldn't find radio?");
    while (1);
  }

  // Uncomment to scan power of entire range from 87.5 to 108.0 MHz
  /*
  for (uint16_t f  = 8750; f<10800; f+=10) {
   radio.readTuneMeasure(f);
   Serial.print("Measuring "); Serial.print(f); Serial.print("...");
   radio.readTuneStatus();
   Serial.println(radio.currNoiseLevel);
   }
   */

  Serial.print("\nSet TX power");
  tft.print("\nSet TX power");
  radio.setTXpower(115);  // dBuV, 88-115 max

  Serial.print("\nTuning into ");
  Serial.print(FMSTATION/100);
  Serial.print('.');
  Serial.println(FMSTATION % 100);
  tft.print('.');
  tft.print("\nTuning into ");
  tft.print(FMSTATION/100);
  tft.println(FMSTATION % 100);
  radio.tuneFM(FMSTATION); // 102.3 mhz

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
  radio.setRDSstation("AdaRadio");
  radio.setRDSbuffer( "Adafruit g0th Radio!");

  Serial.println("RDS on!");
  tft.println("RDS on!");
  // radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output
}

void fm_loop() {
  radio.readASQ();
  Serial.print("\tCurr ASQ: 0x");
  Serial.println(radio.currASQ, HEX);
  tft.print("\tCurr ASQ: 0x");
  tft.println(radio.currASQ, HEX);
  Serial.print("\tCurr InLevel:");
  Serial.println(radio.currInLevel);
  tft.print("\tCurr InLevel:");
  tft.println(radio.currInLevel);
  // toggle GPO1 and GPO2
  // radio.setGPIO(_BV(1));
  delay(500);
  // radio.setGPIO(_BV(2));
  delay(500);
}
