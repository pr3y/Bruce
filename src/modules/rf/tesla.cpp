#include "tesla.h"
#include "rf_utils.h"

String tesla_region = "EU AS AUS";
bool tesla_cc1101 = false;
int tesla_tx_pin = 0;

const uint8_t tesla_sequence[] = {0x02, 0xAA, 0xAA, 0xAA, 0x2B, 0x2C, 0xCB, 0x33, 0x33, 0x2D, 0x34,
                                  0xB5, 0x2B, 0x4D, 0x32, 0xAD, 0x2C, 0x56, 0x59, 0x96, 0x66, 0x66,
                                  0x5A, 0x69, 0x6A, 0x56, 0x9A, 0x65, 0x5A, 0x58, 0xAC, 0xB3, 0x2C,
                                  0xCC, 0xCC, 0xB4, 0xD2, 0xD4, 0xAD, 0x34, 0xCA, 0xB4, 0xA0};
const uint8_t tesla_sequence_length = sizeof(tesla_sequence);

void tesla_send_byte(uint8_t dataByte) {
    for (int8_t bit = 7; bit >= 0; bit--) {
        digitalWrite(tesla_tx_pin, (dataByte & (1 << bit)) ? HIGH : LOW);
        delayMicroseconds(400);
    }
}

void tesla_options() {
    const String list_cc1101[] = {"EU AS AUS", "US", "EXIT"};
    const String list_generic_module[] = {"RUN", "EXIT"};

    const String *region_list;
    int arraySize;

    if (tesla_cc1101) {
        region_list = list_cc1101;
        arraySize = sizeof(list_cc1101) / sizeof(list_cc1101[0]);
    } else {
        region_list = list_generic_module;
        arraySize = sizeof(list_generic_module) / sizeof(list_generic_module[0]);
    }

    options = {};
    int ind = 0;

    for (int i = 0; i < arraySize; i++) {
        String tmp = region_list[i];
        options.push_back({tmp.c_str(), [=]() { tesla_region = region_list[i]; }});
    }

    loopOptions(options, ind);
    options.clear();
}

void tesla_start() {
tesla_start_:

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) { tesla_cc1101 = true; }

    tesla_options();

    if (tesla_region == "EXIT") { return; }

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) {
        tesla_tx_pin = bruceConfigPins.CC1101_bus.io0;
        tesla_cc1101 = true;

        float freq = 433.92;
        if (tesla_region == "US") { freq = 315.00; }

        if (!initRfModule("tx", freq)) return;
    } else {
        tesla_tx_pin = bruceConfig.rfTx;
        if (!initRfModule("tx")) return;
    }

    if (tesla_cc1101) { ELECHOUSE_cc1101.setModulation(2); }

    for (uint8_t i = 0; i < 5; i++) {
        String text = String("TESLA | ") + String(tesla_region) + " | " + String((i + 1)) + "/5";
        displayRedStripe(text, getComplementaryColor2(bruceConfig.priColor), bruceConfig.priColor);
        for (uint8_t j = 0; j < tesla_sequence_length; j++) { tesla_send_byte(tesla_sequence[j]); }
        delay(23);
    }

    delay(500);

    goto tesla_start_;
}
