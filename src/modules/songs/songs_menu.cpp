#include "core/display.h"
#include "core/settings.h"
#include "core/mykeyboard.h"
#include "modules/songs/songs.h"

void songs_menu() {
    options = {
        {"Rickroll",       [=]() { rickroll(); }},
        {"GermanySong",    [=]() { was_wollen(); }},
        {"MegaLovania",    [=]() { megalovania(); }},
        {"Doom",           [=]() { doom(); }},
        {"KeyboardCat",    [=]() { kcat(); }},
        {"MerryChristmas", [=]() { christmas(); }},
        {"Main menu",      [=]() { backToMenu(); }},
    };

    delay(200);
    loopOptions(options);
    delay(200);
}