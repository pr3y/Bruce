#ifndef EMV_READER_H
#define EMV_READER_H

#include "PN532.h"
#include <Arduino.h>

typedef struct EMVCard {
    bool parsed = true;
    uint8_t *aid = nullptr;
    size_t pan_len = 0;
    uint8_t *pan = nullptr;
    uint8_t *afl_raw = nullptr;
    uint8_t *validfrom = nullptr;
    uint8_t *validto = nullptr;
} EMVCard;

typedef struct EMVAID {
    uint8_t aid[7];
    const char *name;
} EMVAID;

#define AID_DICT_SIZE 11

// http://hartleyenterprises.com/listAID.html
const EMVAID known_aid[AID_DICT_SIZE] = {
    // MasterCard
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10}, .name = "MasterCard" },
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x04, 0x22, 0x03}, .name = "U.S Maestro"},
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x04, 0x30, 0x60}, .name = "Maestro"    },
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x04, 0x60, 0x00}, .name = "Cirrus"     },
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x04, 0x99, 0x99}, .name = "MasterCard" },
    // Visa
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10}, .name = "Visa"       },
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x03, 0x20, 0x10}, .name = "Electron"   },
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x03, 0x20, 0x20}, .name = "V-Pay"      },
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x03, 0x30, 0x10}, .name = "Visa"       },
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x03, 0x80, 0x10}, .name = "Visa"       },
    {.aid = {0xA0, 0x00, 0x00, 0x00, 0x98, 0x08, 0x40}, .name = "Visa"       }
};

class EMVReader {
private:
    // EMV methods created with the help of https://werner.rothschopf.net/201703_arduino_esp8266_nfc.htm
    void parse_pan(std::vector<uint8_t> *afl_content, EMVCard *card);
    void parse_validfrom(std::vector<uint8_t> *afl_content, EMVCard *card);
    void parse_validto(std::vector<uint8_t> *afl_content, EMVCard *card);
    void get_afl(EMVCard *card, uint8_t *afl);

    // EMV methods created with the help of https://werner.rothschopf.net/201703_arduino_esp8266_nfc.htm
    std::vector<uint8_t> emv_ask_for_aid();
    std::vector<uint8_t> emv_ask_for_app_name();
    std::vector<uint8_t> emv_ask_for_pdol(std::vector<uint8_t> *aid);
    std::vector<uint8_t> emv_ask_for_afl();
    std::vector<uint8_t> emv_read_afl(uint8_t p2);

    PN532 *_rfid;
    Adafruit_PN532 *nfc = nullptr;
    EMVCard read_emv_card();
    void display_emv(EMVCard card);
    void save_emv(const char *aid, const char *pan, const char *validfrom, const char *validto);

public:
    EMVReader() { setup(); };
    ~EMVReader() {};
    void setup();
};

#endif
