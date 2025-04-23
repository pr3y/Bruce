#ifndef HANDYSNIFF_H
#define HANDYSNIFF_H

#include <vector>
#include <functional>
#include "core/menu_items/WifiMenu.h"

// A single menu entry: a text label and a callback action.
struct MenuOption {
    const char* label;
    std::function<void()> action;
};

// Externally visible vector you push options into
extern std::vector<Option> options;

// Entry point: returns true if it connected successfully
bool handysniff_start();

#endif // HANDYSNIFF_H
