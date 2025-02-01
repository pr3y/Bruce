#include "core/display.h"
#include "core/settings.h"
#include "core/mykeyboard.h"
#include "morsecode_menu.h"
#include "core/config.h"
#include "core/utils.h"

#ifdef HAS_RGB_LED
    #include <FastLED.h>
    #include "core/led_control.h"
#endif

const char* morseCode[] = {
    ".-",   "-...", "-.-.", "-..",  ".",    "..-.", "--.",  "....", "..",   // A-I
    ".---", "-.-",  ".-..", "--",   "-.",   "---",  ".--.", "--.-", ".-.",  // J-R
    "...",  "-",    "..-",  "...-", ".--",  "-..-", "-.--", "--..",         // S-Z
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", // 0-7
    "---..", "----."                                                    // 8-9
};

String getMorse(char c) {
    if (c >= 'A' && c <= 'Z') return String(morseCode[c - 'A']);
    if (c >= 'a' && c <= 'z') return String(morseCode[c - 'a']);
    if (c >= '0' && c <= '9') return String(morseCode[c - '0' + 26]);
    if (c == ' ') return " ";  // Space between words
    return "";  // Ignore unknown characters
}

void flashMorse(const String &message)
{
    for (int i = 0; i < message.length(); i++)
    {
        String morse = getMorse(message[i]);
        for (int j = 0; j < morse.length(); j++)
        {
            #ifdef T_EMBED_1101
                if (bruceConfig.ledColor == CRGB::Black)
                {
                    setLedColor(CRGB::White);
                }
                else
                {
                    setLedColor(bruceConfig.ledColor);
                }

                if (morse[j] == '.')
                {
                    delay(200); // Short flash for dot
                }
                else if (morse[j] == '-')
                {
                    delay(600); // Longer flash for dash
                }
                setLedColor(CRGB::Black);
                delay(200); // Space between dots/dashes
            #endif
        }
        delay(600); // Space between letters
    }
}

void generate_morsecode(String plainText)
{
    #ifdef HAS_SCREEN
        drawMainBorder(true);
        tft.setTextSize(1);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawCentreString("SENDING MORSE SIGNALS", tftWidth / 2, tftHeight / 2, 1);
    #endif

    #ifdef T_EMBED_1101
        beginLed();
        flashMorse(plainText);
        bruceConfig.ledColor == CRGB::Black ? setLedColor(CRGB::Black) : setLedColor(bruceConfig.ledColor);
        morsecode_menu();
    #endif

    #ifdef HAS_SCREEN
        delay(300); // Due to M5 sel press, it could be confusing with next line
        while (!check(EscPress) && !check(SelPress))
            delay(100);
    #endif
}

void custom_morsecode()
{
    String message = keyboard("", 100, "Plain Text:");
    return generate_morsecode(message);
}

void save_custom_morsecode()
{
    String name = keyboard("", 100, "Preset Name:");
    if (name.isEmpty())
    {
        displayError("Name cannot be empty!");
        delay(1000);
        return;
    }

    if (std::any_of(bruceConfig.morseCodes.begin(), bruceConfig.morseCodes.end(),
                    [&](const BruceConfig::MorseCodeEntry &entry)
                    {
                        return entry.menuName == name;
                    }))
    {
        displayError("Code already exists!");
        delay(1000);
        return;
    }

    String text = keyboard("", 100, "Plain text:");

    bruceConfig.addMorseCodeEntry(name, text);
    return generate_morsecode(text);
}

void remove_custom_morsecode() {
    if (bruceConfig.morseCodes.empty()){
        displayInfo("There is nothing to remove!");
        delay(1000);
        custom_morsecode_menu();
    }
    std::vector<Option> options;

    // Populate options with the QR codes from the config
    for (const auto& entry : bruceConfig.morseCodes) {
        options.emplace_back(
            std::string(entry.menuName.c_str()), 
            [=]() { 
                bruceConfig.removeMorseCodeEntry(entry.menuName); 
                log_i("Removed Morse code: %s", entry.menuName.c_str());
                custom_morsecode_menu(); 
            }
        );
    }

    options.emplace_back("Back", [=]() { custom_morsecode_menu(); });

    loopOptions(options);
}  

void custom_morsecode_menu()
{
    options = { {"One-Time Code", [=]() { custom_morsecode(); }},
                {"Save Preset", [=]() { save_custom_morsecode(); }},
                {"Remove",       [=]() { remove_custom_morsecode(); }},
                {"Back",     [=]() { morsecode_menu(); }}};
    loopOptions(options);
}

void preset_morsecode(String preset) {
    return generate_morsecode(preset);
}

void morsecode_menu()
{
    std::vector<Option> options;
    options.emplace_back("S.O.S", [=]() { preset_morsecode("SOS"); });
    options.emplace_back("9.1.1", [=]() { preset_morsecode("911"); });

    // Add QR codes from the config
    for (const auto& entry : bruceConfig.morseCodes) {
        options.emplace_back(std::string(entry.menuName.c_str()), [=]() { generate_morsecode(entry.content); });
    }

    options.emplace_back("Custom Code", [=]() { custom_morsecode_menu(); });
    options.emplace_back("Main menu", [=]() { backToMenu(); });
    loopOptions(options);
}