/**
 * @file ir_read.cpp
 * @author @im.nix (https://github.com/Niximkk)
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read IR signals
 * @version 0.2
 * @date 2024-08-03
 */

#include <IRrecv.h>
#include <IRutils.h>
#include "ir_read.h"
#include "core/globals.h"
#include "core/mykeyboard.h"
#include "core/display.h"
#include "core/sd_functions.h"
#include "core/settings.h"

/* Dont touch this */
//#define MAX_RAWBUF_SIZE 300
#define IR_FREQUENCY 38000
#define DUTY_CYCLE 0.330000


String uint32ToString(uint32_t value) {
  char buffer[12] = {0};  // 8 hex digits + 3 spaces + 1 null terminator
  snprintf(buffer, sizeof(buffer), "%02X %02X %02X %02X",
           value & 0xFF,
           (value >> 8) & 0xFF,
           (value >> 16) & 0xFF,
           (value >> 24) & 0xFF);
  return String(buffer);
}

String uint32ToStringInverted(uint32_t value) {
  char buffer[12] = {0};  // 8 hex digits + 3 spaces + 1 null terminator
  snprintf(buffer, sizeof(buffer), "%02X %02X %02X %02X",
           (value >> 24) & 0xFF,
           (value >> 16) & 0xFF,
           (value >> 8) & 0xFF,
           value & 0xFF);
  return String(buffer);
}



IrRead::IrRead(bool headless_mode, bool raw_mode) {
    headless = headless_mode;
    raw = raw_mode;
    setup();
}

void IrRead::setup() {
    irrecv.enableIRIn();

    //Checks if irRx pin is properly set
    const std::vector<std::pair<std::string, int>> pins = IR_RX_PINS;
    int count=0;
    for (auto pin : pins) {
        if(pin.second==bruceConfig.irRx) count++;
    }
    if(count==0) gsetIrRxPin(true); // Open dialog to choose irRx pin

    pinMode(bruceConfig.irRx, INPUT);
    if(headless) return;
    // else
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
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
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

void IrRead::read_signal() {
    if (_read_signal || !irrecv.decode(&results)) return;

    _read_signal = true;

    // switch to raw mode if decoding failed
    if(results.decode_type == decode_type_t::UNKNOWN ) {
        displayWarning("signal decoding failed, switching to RAW mode", true);
        raw = true;
        // TODO: show a dialog
        // raw = yesNoDialog("decoding failed, save as RAW?");
    }

    display_banner();

    // dump signal details
    padprint("HEX: 0x");
    tft.println(results.value, HEX);

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

    append_to_file_str(btn_name);

    signals_read++;

    discard_signal();
    delay(100);
}


String IrRead::parse_state_signal() {
    String r = "";
    uint16_t state_len = (results.bits) / 8;
    for (uint16_t i = 0; i < state_len; i++) {
        //r += uint64ToString(results.state[i], 16) + " ";
        r += ((results.state[i] < 0x10) ? "0" : "");  // adds 0 padding if necessary
        r += String(results.state[i], HEX) + " ";
    }
    r.toUpperCase();
    return r;
}

String IrRead::parse_raw_signal() {
    // https://github.com/crankyoldgit/IRremoteESP8266/blob/master/examples/SmartIRRepeater/SmartIRRepeater.ino
    rawcode = resultToRawArray(&results);
    raw_data_len = getCorrectedRawLength(&results);

    String signal_code = "";

    for (uint16_t i = 0; i < raw_data_len; i++) {
        signal_code += String(rawcode[i]) + " ";
    }

    delete[] rawcode;
    rawcode = nullptr;
    signal_code.trim();

    return signal_code;
}


void IrRead::append_to_file_str(String btn_name) {
    strDeviceContent += "name: " + btn_name + "\n";

    if(raw) {
        strDeviceContent += "type: raw\n";
        strDeviceContent += "frequency: " + String(IR_FREQUENCY) + "\n";
        strDeviceContent += "duty_cycle: " + String(DUTY_CYCLE) + "\n";
        strDeviceContent += "data: " + parse_raw_signal() + "\n";
    } else {
        // parsed signal  https://github.com/jamisonderek/flipper-zero-tutorials/wiki/Infrared
        strDeviceContent +=  "type: parsed\n";
        switch (results.decode_type) {
            case decode_type_t::RC5:
            {
                if(results.command > 0x3F )
                    strDeviceContent += "protocol: RC5X\n";
                else
                    strDeviceContent += "protocol: RC5\n";
                break;
            }
            case decode_type_t::RC6:
            {
                strDeviceContent += "protocol: RC6\n";
                break;
            }
            case decode_type_t::SAMSUNG:
            {
                strDeviceContent += "protocol: Samsung32\n";
                break;
            }
            case decode_type_t::SONY:
            {
                // check address and command ranges to find the exact protocol
                if(results.address>0xFF)
                    strDeviceContent += "protocol: SIRC20\n";
                else if(results.address>0x1F)
                    strDeviceContent += "protocol: SIRC15\n";
                else
                    strDeviceContent += "protocol: SIRC\n";
                break;
            }
            case decode_type_t::NEC:
            {
                // check address and command ranges to find the exact protocol
                if(results.address>0xFFFF)
                    strDeviceContent +=  "protocol: NEC42ext\n";
                else if(results.address>0xFF1F)
                    strDeviceContent +=  "protocol: NECext\n";
                else if(results.address>0xFF)
                    strDeviceContent +=  "protocol: NEC42\n";
                else
                    strDeviceContent +=  "protocol: NEC\n";
                break;
            }
            case decode_type_t::UNKNOWN:
            {
                Serial.print("unknown protocol, try raw mode");
                return;
            }
            default:
            {
                strDeviceContent +=  "protocol: " + typeToString(results.decode_type, results.repeat) + "\n";
                break;
            }
        }

        strDeviceContent +=  "address: " + uint32ToString(results.address) + "\n";
        strDeviceContent +=  "command: " + uint32ToString(results.command) + "\n";

        // extra fields not supported on flipper
        strDeviceContent +=  "bits: " + String(results.bits) + "\n";
        if(hasACState(results.decode_type))
            strDeviceContent +=  "state: " + parse_state_signal() + "\n";
        else if(results.bits>32)
            strDeviceContent +=  "value: " + uint32ToString(results.value) + " " + uint32ToString(results.value>> 32) + "\n";  // MEMO: from uint64_t
        else
            strDeviceContent +=  "value: " + uint32ToStringInverted(results.value) + "\n";

        /*
        Serial.println(results.bits);
        Serial.println(results.address);
        Serial.println(results.command);
        Serial.println(results.overflow);
        Serial.println(results.repeat);
        Serial.println("value:");
        serialPrintUint64(results.address, HEX);
        serialPrintUint64(results.command, HEX);
        Serial.print("resultToHexidecimal: ");
        Serial.println(resultToHexidecimal(&results));
        Serial.println(results.value);
        String value = uint32ToString(results.value ) + " " + uint32ToString(results.value>> 32);
        value.replace(" ", "");
        uint64_t value_int = strtoull(value.c_str(), nullptr, 16);
        Serial.println(value_int);
        */
    }
    strDeviceContent += "#\n";
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
        displaySuccess("File saved to " + String((fs == &SD) ? "SD Card" : "LittleFS") + ".", true);
        signals_read = 0;
        strDeviceContent = "";
    } else {
        if (fs == nullptr) {
            displayError("No storage available.", true);
        } else displayError("Error writing file.", true);
    }

    delay(1000);
    begin();
}


String IrRead::loop_headless(int max_loops) {

    while (!irrecv.decode(&results)) {  // MEMO: default timeout is 15ms
        max_loops -= 1;
        if(max_loops <= 0) {
            Serial.println("timeout");
            return "";  // nothing received
        }
        delay(1000);
        //delay(50);
    }

    irrecv.disableIRIn();

    if(!raw && results.decode_type == decode_type_t::UNKNOWN )
    {
        Serial.println("# decoding failed, try raw mode");
        return "";
    }

    if(results.overflow) displayWarning("buffer overflow, data may be truncated", true);
    // TODO: check results.repeat

    String r = "Filetype: IR signals file\n";
    r += "Version: 1\n";
    r += "#\n";
    r += "#\n";

    strDeviceContent = "";
    append_to_file_str("Unknown");  // writes on strDeviceContent
    r += strDeviceContent;

    return r;
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
    file.println("Version: 1");
    file.println("#");
    file.println("# " + filename);
    file.print(strDeviceContent);

    file.close();
    delay(100);
    return true;
}
