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
    digitalWrite(nTransmitterPin, HIGH);
    const unsigned long TIMEOUT_MS = 10UL * 60UL * 1000UL; // 10 minutes timeout
    unsigned long tmr0 = millis();

    while (sendRF) {
        if (check(EscPress) || (millis() - tmr0 > TIMEOUT_MS)) {
            sendRF = false;
            returnToMenu = true;
            break;
        }
    }
    digitalWrite(nTransmitterPin, LOW);
}

void RFJammer::run_itmt_jammer() {
    const unsigned long TIMEOUT_MS = 10UL * 60UL * 1000UL; // 10 minutes
    unsigned long tmr0 = millis();

    while (sendRF) {
        for (int sequence = 1; sequence < 50; sequence++) {
            for (int duration = 1; duration <= 3; duration++) {
                // Moved Escape check into this loop to check every cycle
                if (check(EscPress) || (millis() - tmr0 > TIMEOUT_MS)) {
                    sendRF = false;
                    returnToMenu = true;
                    break;
                }
                digitalWrite(nTransmitterPin, HIGH); // Ativa o pino
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) { delayMicroseconds(10); }

                digitalWrite(nTransmitterPin, LOW); // Desativa o pino
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) { delayMicroseconds(10); }
            }
        }
    }
    digitalWrite(nTransmitterPin, LOW); // Turn off Jammer
}
