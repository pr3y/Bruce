// Globals.h

#define FGCOLOR TFT_PURPLE+0x3000
#define ALCOLOR TFT_RED
#define BGCOLOR TFT_BLACK

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <functional>
#include <vector>


#if defined (STICK_C_PLUS) || defined (STICK_C) 
  #include <AXP192.h>
  extern AXP192 axp192;
#endif

#if defined(CARDPUTER)
  #include <Keyboard.h>
  extern Keyboard_Class Keyboard;
#endif

// Declaração dos objetos TFT
extern TFT_eSPI tft; 
extern TFT_eSprite sprite;
extern TFT_eSprite menu_op;
extern TFT_eSprite draw;

extern int prog_handler;    // 0 - Flash, 1 - SPIFFS, 2 - Download

extern bool sdcardMounted;  // informa se o cartão está montado ou não, sem precisar chamar a função setupSdCard

extern bool wifiConnected;  // informa se o wifi está ativo ou não

extern bool BLEConnected;  // informa se o BLE está ativo ou não

extern std::vector<std::pair<std::string, std::function<void()>>> options;

extern  String ssid;

extern  String pwd;

extern String fileToCopy;

extern int rotation;

extern uint8_t buff[4096];

extern const int bufSize;

extern bool returnToMenu; // variável para verificação e quebrar os loops

void backToMenu();

