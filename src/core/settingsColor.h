#ifndef COLOR_STORAGE_H
#define COLOR_STORAGE_H

#include <globals.h>

#define LIGHT_BLUE     0x96FE
#define DARK_GREY      0x1082
#define EAGLE          0xB591
#define CELADON        0xA7B6
#define TROPICAL_BLUE  0xA61D
#define PEAR           0xD7A6
#define SHOCKING_PINK  0xDD39
#define OLD_ROSE       0xC189
#define OLIVE_GREEN    0xBD8A
#define VIOLET         0xAAD4
#define MIDNIGHT_BLUE  0x190C
#define PURPLE         0x594F
#define ALIZARIN       0xE8E7
#define FINN           0x69CA

struct ColorEntry {
    const char *name;
    uint16_t    value;
};

static constexpr ColorEntry UI_COLORS[] = {
    { "Default",        DEFAULT_PRICOLOR },
    { "White",          TFT_WHITE       },
    { "Red",            TFT_RED         },
    { "Green",          TFT_DARKGREEN   },
    { "Blue",           TFT_BLUE        },
    { "Light Blue",     LIGHT_BLUE      },
    { "Yellow",         TFT_YELLOW      },
    { "Magenta",        TFT_MAGENTA     },
    { "Orange",         TFT_ORANGE      },
    { "Grey",           DARK_GREY       },
    { "Eagle",          EAGLE           },
    { "Celadon",        CELADON         },
    { "Tropical Blue",  TROPICAL_BLUE   },
    { "Pear",           PEAR            },
    { "Shocking Pink",  SHOCKING_PINK   },
    { "Old Rose",       OLD_ROSE        },
    { "Olive Green",    OLIVE_GREEN     },
    { "Violet",         VIOLET          },
    { "Midnight Blue",  MIDNIGHT_BLUE   },
    { "Purple",         PURPLE          },
    { "Alizarin",       ALIZARIN        },
    { "Finn",           FINN            }
};

static constexpr int UI_COLOR_COUNT =
    sizeof(UI_COLORS) / sizeof(UI_COLORS[0]);

#endif // COLOR_STORAGE_H
