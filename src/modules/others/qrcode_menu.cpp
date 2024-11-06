#if !defined(M5STACK) || defined(CORE) || defined(CORE2)
#include "../lib/TFT_eSPI_QRcode/src/qrcode.h"
#endif
#include "core/display.h"
#include "core/settings.h"
#include "core/mykeyboard.h"


uint16_t crc_ccitt_update(uint16_t crc, uint8_t data) {
    crc = (uint8_t)(crc >> 8) | (crc << 8);
    crc ^= data;
    crc ^= (uint8_t)(crc & 0xff) >> 4;
    crc ^= crc << 12;
    crc ^= (crc & 0x00ff) << 5;
    return crc;
}


String calculate_crc(String input) {
    size_t len = input.length();
    uint8_t *data = (uint8_t *)input.c_str();
    uint16_t crc = 0xffff;

    for (size_t i = 0; i < len; i++) {
        crc = crc_ccitt_update(crc, data[i]);
    }

    String crc_str = String(crc, HEX);
    crc_str.toUpperCase();
    while (crc_str.length() < 4) crc_str = "0" + crc_str; // Pad with zeros if needed

    return crc_str;
}


void qrcode_display(String qrcodeUrl) {
#ifdef HAS_SCREEN
  #if defined(M5STACK) && !defined(CORE2) && !defined(CORE)
    tft.qrcode(qrcodeUrl,5,5,HEIGHT);
  #else
    QRcode qrcode(&tft);
    qrcode.init();

    qrcode.create(qrcodeUrl);
  #endif
    delay(300); //Due to M5 sel press, it could be confusing with next line
    while(!checkEscPress() && !checkSelPress()) delay(100);
    tft.fillScreen(bruceConfig.bgColor);
#endif
}

void custom_qrcode() {
    String message = keyboard("", 100, "QRCode text:");
    return qrcode_display(message);
}

void pix_qrcode() {
    String key = keyboard("", 25, "PIX Key:");
    String key_length = key.length() >= 10 ? String(key.length()) : "0" + String(key.length());
    String amount = keyboard("1000.00", 10, "Int amount:");
    amount = String(amount.toFloat());
    String amount_length = amount.length() >= 10 ? String(amount.length()) : "0" + String(amount.length());

    String data0 = "0014BR.GOV.BCB.PIX01" + key_length + key;

    String pix_code = "00020126" + String(data0.length()) + data0 + "52040000530398654" + amount_length + amount + "5802BR5909Bruce PIX6014Rio de Janeiro62070503***6304";
    String crc = calculate_crc(pix_code);

    return qrcode_display(pix_code + crc);
}

void qrcode_menu() {
    options = {
        {"Bruce AP",    [=]() { qrcode_display("WIFI:T:WPA;S:BruceNet;P:brucenet;;"); }},
        {"Bruce Repo",  [=]() { qrcode_display("https://github.com/pr3y/Bruce"); }},
        {"Rickroll",    [=]() { qrcode_display("https://youtu.be/dQw4w9WgXcQ"); }},
        {"HackerTyper", [=]() { qrcode_display("https://hackertyper.net/"); }},
        {"ZomboCom",    [=]() { qrcode_display("https://html5zombo.com/"); }},
        {"PIX",         [=]() { pix_qrcode(); }},
        {"Custom",      [=]() { custom_qrcode(); }},
        {"Main menu",   [=]() { backToMenu(); }},
    };

    delay(200);
    loopOptions(options);
    delay(200);
}

