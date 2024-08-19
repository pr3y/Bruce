/**
 * @file ir_read.cpp
 * @author @im.nix (https://github.com/Niximkk)
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read IR signals
 * @version 0.2
 * @date 2024-08-03
 */

#include <IRrecv.h>
#include "ir_read.h"
#include "core/globals.h"
#include "core/mykeyboard.h"
#include "core/display.h"
#include "core/sd_functions.h"
#include "core/settings.h"

/* Dont touch this */
#define MAX_RAWBUF_SIZE 300
#define IR_FREQUENCY 38000
#define DUTY_CYCLE 0.330000


IrRead::IrRead() {
    setup();
}

void IrRead::setup() {
    irrecv.enableIRIn();

    //Checks if IrRx pin is properly set
    const std::vector<std::pair<std::string, int>> pins = IR_RX_PINS;
    int count=0;
    for (auto pin : pins) {
        if(pin.second==IrRx) count++; 
    }
    if(count==0) gsetIrRxPin(true); // Open dialog to choose IrRx pin
    
    pinMode(IrRx, INPUT);
    begin();
    return loop();
}

void IrRead::loop() {
    while(1) {
        if (checkEscPress()) {
            returnToMenu=true;
            break;
        }

        if (checkSelPress()) save_device();
        if (checkNextPress()) save_signal();
        if (checkPrevPress()) discard_signal();

        read_signal();
    }
}

void IrRead::begin() {
    _read_signal = false;

    display_banner();
    padprintln("Waiting for signal...");
    tft.println("");
    display_btn_options();

    delay(300);
}

void IrRead::cls() {
    drawMainBorder();
    tft.setCursor(10, 28);
    tft.setTextColor(FGCOLOR, BGCOLOR);
}

void IrRead::display_banner() {
    cls();
    tft.setTextSize(FM);
    padprintln("IR Read");

    tft.setTextSize(FP);
    padprintln("--------------");
    padprintln("Signals captured: " + String(signals_read));
    tft.println("");
}

void IrRead::display_btn_options() {
    tft.println("");
    tft.println("");
    if (_read_signal) {
        padprintln("Press [PREV] to discard signal");
        padprintln("Press [NEXT] to save signal");
    }
    if (signals_read > 0) {
	    padprintln("Press [OK]   to save device");
    }
	padprintln("Press [ESC]  to exit");
}

void IrRead::dump_signal_details() {
    padprint("HEX: 0x");
    tft.println(results.value, HEX);
}

void IrRead::read_signal() {
    if (_read_signal || !irrecv.decode(&results)) return;

    _read_signal = true;

    display_banner();
    dump_signal_details();
    display_btn_options();

    delay(500);
}

void IrRead::discard_signal() {
    if (!_read_signal) return;

    irrecv.resume();
    begin();
}

void IrRead::save_signal() {
    if (!_read_signal) return;

    String btn_name = keyboard("Btn"+String(signals_read), 30, "Btn name:");

    append_to_file_str(btn_name, parse_signal());

    signals_read++;

    discard_signal();
    delay(100);
}

String IrRead::parse_signal() {
    rawcode = new uint16_t[MAX_RAWBUF_SIZE];
    memset(rawcode, 0, MAX_RAWBUF_SIZE * sizeof(uint16_t));
    raw_data_len = results.rawlen;
    String signal_code = "";

    /* I HAVE NO FUCKING IDEA WHY WE NEED TO MULTIPLY BY 2, BUT WE DO. */
    for (int i = 1; i < raw_data_len; i++) {
        signal_code += String(results.rawbuf[i] * 2) + " ";
        rawcode[i - 1] = results.rawbuf[i] * 2;
    }
    delete[] rawcode;
    rawcode = nullptr;
    signal_code.trim();

    return signal_code;
}

void IrRead::append_to_file_str(String btn_name, String signal_code) {
    strDeviceContent += "#\n";
    strDeviceContent += "name: " + btn_name + "\n";
    strDeviceContent += "type: raw\n";
    strDeviceContent += "frequency: " + String(IR_FREQUENCY) + "\n";
    strDeviceContent += "duty_cycle: " + String(DUTY_CYCLE) + "\n";
    strDeviceContent += "data: " + String(signal_code) + "\n";
}

void IrRead::save_device() {
    if (signals_read == 0) return;

    String filename = keyboard("MyDevice", 30, "File name:");

    display_banner();
    
    FS* fs = nullptr;

    bool sdCardAvaible = setupSdCard();
    bool littleFsAvaible = checkLittleFsSize();

    if (sdCardAvaible && littleFsAvaible) {
        // ask to choose one
        options = {
            {"SD Card", [&]()    { fs=&SD; }},
            {"LittleFS", [&]()   {  fs=&LittleFS; }},
        };
        delay(200);
        loopOptions(options);
    } else if (sdCardAvaible) {
        fs=&SD;
    } else if (littleFsAvaible) {
        fs=&LittleFS;
    };

    if (fs != nullptr && write_file(filename, fs)) {
        displaySuccess("File saved to " + String((fs == &SD) ? "SD Card" : "LittleFS") + ".");
        signals_read = 0;
        strDeviceContent = "";
    } else {
        if (fs == nullptr) {
            displayError("No storage available.");
        } else displayError("Error writing file.");
    }

    delay(1000);
    begin();
}

bool IrRead::write_file(String filename, FS* fs) {
    if (fs == nullptr) return false;

    if (!(*fs).exists("/BruceIR")) (*fs).mkdir("/BruceIR");
    if ((*fs).exists("/BruceIR/" + filename + ".ir")) {
        int i = 1;
        filename += "_";
        while((*fs).exists("/BruceIR/" + filename + String(i) + ".ir")) i++;
        filename += String(i);
    }
    File file = (*fs).open("/BruceIR/"+ filename + ".ir", FILE_WRITE);

    if(!file) {
        return false;
    }

    file.println("Filetype: Bruce IR File");
    file.println("Version 1");
    file.println("#");
    file.println("# " + filename);
    file.print(strDeviceContent);

    file.close();
    delay(100);
    return true;
}
