#ifndef COLOR_STORAGE_H
#define COLOR_STORAGE_H
#include <globals.h> //for main colors


#define LIGHT_BLUE 0x96FE
#define DARK_GREY 0x1082
#define Eagle 0xB591
#define Celadon 0xA7B6
#define Tropical_Blue 0xA61D
#define Pear 0xD7A6
#define Shocking_Pink 0xDD39
#define Old_Rose 0xC189
#define Olive_Green 0xBD8A
#define Violet 0xAAD4
#define Midnight_Blue 0x190C
#define Purple 0x594F
#define Alizarin 0xE8E7
#define Finn 0x69CA
#define test 0x07FF


static const char *UI_COLOR_NAMES[] = {
    "Default", 
    "White",    "Red",         "Green",  "Blue",          "Light Blue",    "Yellow",   "Magenta",
    "Orange",   "Grey",        "Eagle",  "Celadon",       "Tropical Blue", "Pear",     "Shocking Pink",
    "Old Rose", "Olive Green", "Violet", "Midnight Blue", "Purple",        "Alizarin", "Finn", "test"
};

static const uint16_t UI_COLOR_VALUES[] = {
    DEFAULT_PRICOLOR, 
    TFT_WHITE,        TFT_RED,     TFT_DARKGREEN, TFT_BLUE,      LIGHT_BLUE,    TFT_YELLOW, TFT_MAGENTA,
    TFT_ORANGE,       DARK_GREY,   Eagle,         Celadon,       Tropical_Blue, Pear,       Shocking_Pink,
    Old_Rose,         Olive_Green, Violet,        Midnight_Blue, Purple,        Alizarin,   Finn, test
};

// Number of built-in entries. (Never modify this by hand!)
static const int UI_COLOR_COUNT = sizeof(UI_COLOR_VALUES) / sizeof(UI_COLOR_VALUES[0]);

#endif // COLOR_STORAGE_H
