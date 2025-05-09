#include "rf_jammer.h"
#include "core/display.h"
#include "rf_utils.h"

RFJammer::RFJammer(bool full) : fullJammer(full) { setup(); }

RFJammer::~RFJammer() { deinitRfModule(); }

void RFJammer::setup() {
    nTransmitterPin = bruceConfig.rfTx;
    if (!initRfModule("tx")) return;

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) { // CC1101 in use
        nTransmitterPin = bruceConfigPins.CC1101_bus.io0;
    }

    bool sendRF = true;

    display_banner();

    if (fullJammer) return run_full_jammer();
    else run_itmt_jammer();
}

void RFJammer::display_banner() {
    drawMainBorderWithTitle("RF Jammer");
    printSubtitle(String(fullJammer ? "Full Jammer" : "Intermittent Jammer"));

    padprintln("Sending...");
    padprintln("");
    padprintln("");

    tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
    padprintln("Press [ESC] for options.");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
}

void RFJammer::run_full_jammer() {
    digitalWrite(nTransmitterPin, HIGH); // Turn on Jammer
    int tmr0 = millis();                 // control total jammer time;

    while (sendRF) {
        if (check(EscPress) || (millis() - tmr0 > 20000)) {
            sendRF = false;
            returnToMenu = true;
            break;
        }
    }
}

void RFJammer::run_itmt_jammer() {
    int tmr0 = millis();

    while (sendRF) {
        for (int sequence = 1; sequence < 50; sequence++) {
            for (int duration = 1; duration <= 3; duration++) {
                // Moved Escape check into this loop to check every cycle
                if (check(EscPress) || (millis() - tmr0) > 20000) {
                    sendRF = false;
                    returnToMenu = true;
                    break;
                }
                digitalWrite(nTransmitterPin, HIGH); // Ativa o pino
                // keeps the pin active for a while and increase increase
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) { delayMicroseconds(10); }

                digitalWrite(nTransmitterPin, LOW); // Desativa o pino
                // keeps the pin inactive for the same time as before
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) { delayMicroseconds(10); }
            }
        }
    }
}
