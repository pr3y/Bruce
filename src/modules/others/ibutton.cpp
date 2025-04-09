// thanks geo-tp for the base of this
#include "ibutton.h"
#include "core/display.h"
#include "core/mykeyboard.h"

#define ONE_WIRE_BUS 0

OneWire *oneWire;
byte buffer[8];

void setup_ibutton() {
Reset:
    oneWire = new OneWire(bruceConfig.iButton);
    tft.fillScreen(TFT_BLACK);
    setiButtonPinMenu();
    drawMainBorderWithTitle("iButton");
    tft.setCursor(10, 50);
    padprintln("Waiting for signal");
    padprintln("press [Next] to setup");
    delay(100);

    for (;;) {
        if (check(EscPress)) {
            returnToMenu = true;
            delete oneWire;
            break;
        }
        if (check(NextPress)) {
            setiButtonPinMenu();
            delete oneWire;
            goto Reset;
        }
        // iButton is plugged
        if (oneWire->reset() != 0) {
            // Main Button is pressed
            if (check(SelPress)) {
                write_ibutton();
            } else {
                read_ibutton();
            }
            delay(500);
        }
    }
}

void write_byte_rw1990(byte data) {
    for (int data_bit = 0; data_bit < 8; data_bit++) {
        delay(25);
        oneWire->write_bit(~data);
        data >>= 1;
    }
}

void write_ibutton() {
    tft.setCursor(52, 102);
    tft.print("Wait...");

    oneWire->reset(); // Reset bus
    oneWire->skip();  // Skip rom check
    delay(20);

    // Step 1 : Prepare for writing
    oneWire->write(0xD1); // Start write command
    oneWire->reset();     // Reset bus

    // Step 2 : Write the ID
    oneWire->write(0xD5); // Write ID command
    for (byte i = 0; i < 8; i++) {
        write_byte_rw1990(buffer[i]); // Write each byte
    }
    oneWire->reset(); // Reset bus

    // Step 3 : Finalise
    oneWire->write(0xD1); // End of write command
    oneWire->reset();     // Reset bus

    // Display end of copy
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(90, 50);
    tft.setTextSize(2);
    displayTextLine("COPIED");
    tft.setCursor(40, 80);
    tft.print("Release button");

    delay(3000);

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 60);
    displayTextLine("Waiting iButton...");
}

void read_ibutton() {
    oneWire->write(0x33);           // Read ID command
    oneWire->read_bytes(buffer, 8); // Read ID

    // Display iButton
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(55, 20);
    tft.println("iButton ID: ");

    // Dislay ID
    tft.setTextSize(1.7);
    tft.setCursor(12, 57);
    for (byte i = 0; i < 8; i++) {
        tft.print(buffer[i], HEX);
        tft.print(" ");
    }

    // Display copy infos
    tft.setCursor(55, 85);
    tft.setTextSize(1.5);
    tft.println("Hold OK to copy");
}

/*********************************************************************
**  Function: setiButtonPin
**  Main Menu to manually iButton Pin
**********************************************************************/
void setiButtonPinMenu() {
    options = {};
    gpio_num_t sel = GPIO_NUM_NC;
    for (int8_t i = -1; i <= GPIO_NUM_MAX; i++) {
        String tmp = "GPIO " + String(i);
        options.push_back({tmp.c_str(), [i, &sel]() { sel = (gpio_num_t)i; }});
    }
    loopOptions(options, bruceConfig.iButton + 1);
    options.clear();
    bruceConfig.setiButtonPin(sel);
}
