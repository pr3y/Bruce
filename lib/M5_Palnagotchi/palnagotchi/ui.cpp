#include "ui.h"

M5Canvas canvas_top(&M5.Display);
M5Canvas canvas_main(&M5.Display);
M5Canvas canvas_bot(&M5.Display);
// M5Canvas canvas_peers_menu(&M5.Display);

int32_t display_w;
int32_t display_h;
int32_t canvas_h;
int32_t canvas_center_x;
int32_t canvas_top_h;
int32_t canvas_bot_h;
int32_t canvas_peers_menu_h;
int32_t canvas_peers_menu_w;

struct menu {
  char name[25];
  int command;
};

menu main_menu[] = {
    {"Nearby Pwnagotchis", 2},
    // {"Settings", 4},
    {"About", 8}
    // {"Friend spam", 16},
};

menu settings_menu[] = {
    {"Change name", 40},
    {"Display brightness", 41},
    {"Sound", 42},
};

int main_menu_len = sizeof(main_menu) / sizeof(menu);
int settings_menu_len = sizeof(settings_menu) / sizeof(menu);

bool menu_open = false;
uint8_t menu_current_cmd = 0;
uint8_t menu_current_opt = 0;

void initUi() {
  M5.Display.setRotation(1);
  M5.Display.setTextFont(&fonts::Font0);
  M5.Display.setTextSize(1);
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(GREEN);
  M5.Display.setColor(GREEN);

  display_w = M5.Display.width();
  display_h = M5.Display.height();
  canvas_h = display_h * .8;
  canvas_center_x = display_w / 2;
  canvas_top_h = display_h * .1;
  canvas_bot_h = display_h * .1;
  canvas_peers_menu_h = display_h * .8;
  canvas_peers_menu_w = display_w * .8;

  canvas_top.createSprite(display_w, canvas_top_h);
  canvas_bot.createSprite(display_w, canvas_bot_h);
  canvas_main.createSprite(display_w, canvas_h);
}

bool keyboard_changed = false;

bool toggleMenuBtnPressed() {
  // return M5Cardputer.BtnA.isPressed() ||
  //        (keyboard_changed && (M5Cardputer.Keyboard.isKeyPressed('m') ||
  //                              M5Cardputer.Keyboard.isKeyPressed('`')));
  #if defined(STICK_C_PLUS2)
    if(digitalRead(UP_BTN)==LOW)
  #elif defined(STICK_C_PLUS)
    if(axp192.GetBtnPress())
  #elif defined(CARDPUTER)
    Keyboard.update();
    if(Keyboard.isKeyPressed(',') || Keyboard.isKeyPressed(';'))
  #endif
  { return true; }

  else return false;
}

bool isOkPressed() {
  // return M5Cardputer.BtnA.isPressed() ||
  //        (keyboard_changed && M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER));
  #if defined (CARDPUTER)
    Keyboard.update();
    if(Keyboard.isKeyPressed(KEY_ENTER) || digitalRead(0)==LOW)
  #else
    if(digitalRead(SEL_BTN)==LOW)
  #endif
  { return true; }

  else return false;
}

bool isNextPressed() {
  // return keyboard_changed && (M5Cardputer.Keyboard.isKeyPressed('.') ||
  //                             M5Cardputer.Keyboard.isKeyPressed('/') ||
  //                             M5Cardputer.Keyboard.isKeyPressed(KEY_TAB));
  #if defined (CARDPUTER)
    Keyboard.update();
    if(Keyboard.isKeyPressed('/') || Keyboard.isKeyPressed('.'))
  #else
    if(digitalRead(DW_BTN)==LOW)
  #endif
  { return true; }

  else return false;
}
bool isPrevPressed() {
  // return keyboard_changed && (M5Cardputer.Keyboard.isKeyPressed(',') ||
  //                             M5Cardputer.Keyboard.isKeyPressed(';'));
  #if defined(STICK_C_PLUS2)
    if(digitalRead(UP_BTN)==LOW)
  #elif defined(STICK_C_PLUS)
    if(axp192.GetBtnPress())
  #elif defined(CARDPUTER)
    Keyboard.update();
    if(Keyboard.isKeyPressed(',') || Keyboard.isKeyPressed(';'))
  #endif
  { return true; }

  else return false;
}

void updateUi(bool show_toolbars) {
  // #elif defined(CARDPUTER)
  //   keyboard_changed = M5Cardputer.Keyboard.isChange();
  // #endif

  if (toggleMenuBtnPressed()) {
    // If menu is open, return to main menu
    // If not, toggle menu
    if (menu_open == true && menu_current_cmd != 0) {
      menu_current_cmd = 0;
      menu_current_opt = 0;
    } else {
      menu_open = !menu_open;
    }
  }

  uint8_t mood_id = getCurrentMoodId();
  String mood_face = getCurrentMoodFace();
  String mood_phrase = getCurrentMoodPhrase();
  bool mood_broken = isCurrentMoodBroken();

  drawTopCanvas();
  drawBottomCanvas(getPwngridRunTotalPeers(), getPwngridTotalPeers(),
                   getPwngridLastFriendName(), getPwngridClosestRssi());

  if (menu_open) {
    drawMenu();
  } else {
    drawMood(mood_face, mood_phrase, mood_broken);
  }

  M5.Display.startWrite();
  if (show_toolbars) {
    canvas_top.pushSprite(0, 0);
    canvas_bot.pushSprite(0, canvas_top_h + canvas_h);
  }
  canvas_main.pushSprite(0, canvas_top_h);
  M5.Display.endWrite();
}

void drawTopCanvas() {
  canvas_top.fillSprite(BLACK);
  canvas_top.setTextSize(1);
  canvas_top.setTextColor(GREEN);
  canvas_top.setColor(GREEN);
  canvas_top.setTextDatum(top_left);
  canvas_top.drawString("CH *", 0, 3);
  canvas_top.setTextDatum(top_right);
  unsigned long ellapsed = millis() / 1000;
  int8_t h = ellapsed / 3600;
  int sr = ellapsed % 3600;
  int8_t m = sr / 60;
  int8_t s = sr % 60;
  char right_str[50] = "UPS 0%  UP 00:00:00";
  sprintf(right_str, "UPS %i%% UP %02d:%02d:%02d", M5.Power.getBatteryLevel(),
          h, m, s);
  canvas_top.drawString(right_str, display_w, 3);
  canvas_top.drawLine(0, canvas_top_h - 1, display_w, canvas_top_h - 1);
}

String getRssiBars(signed int rssi) {
  String rssi_bars = "";

  if (rssi != -1000) {
    if (rssi >= -67) {
      rssi_bars = "||||";
    } else if (rssi >= -70) {
      rssi_bars = "|||";
    } else if (rssi >= -80) {
      rssi_bars = "||";
    } else {
      rssi_bars = "|";
    }
  }

  return rssi_bars;
}

void drawBottomCanvas(uint8_t friends_run, uint8_t friends_tot,
                      String last_friend_name, signed int rssi) {
  canvas_bot.fillSprite(BLACK);
  canvas_bot.setTextSize(1);
  canvas_bot.setTextColor(GREEN);
  canvas_bot.setColor(GREEN);
  canvas_bot.setTextDatum(top_left);

  // https://github.com/evilsocket/pwnagotchi/blob/2122af4e264495d32ee415c074da8efd905901f0/pwnagotchi/ui/view.py#L191
  String rssi_bars = getRssiBars(rssi);
  char stats[25] = "FRND 0 (0)";
  if (friends_run > 0) {
    sprintf(stats, "FRND %d (%d) [%s] %s", friends_run, friends_tot,
            last_friend_name, rssi_bars);
  }

  canvas_bot.drawString(stats, 0, 5);
  canvas_bot.setTextDatum(top_right);
  canvas_bot.drawString("NOT AI", display_w, 5);
  canvas_bot.drawLine(0, 0, display_w, 0);
}

void drawMood(String face, String phrase, bool broken) {
  if (broken == true) {
    canvas_main.setTextColor(RED);
  } else {
    canvas_main.setTextColor(GREEN);
  }

  canvas_main.setTextSize(4);
  canvas_main.setTextDatum(middle_center);
  canvas_main.fillSprite(BLACK);
  canvas_main.drawString(face, canvas_center_x, canvas_h / 2);
  canvas_main.setTextDatum(bottom_center);
  canvas_main.setTextSize(1);
  canvas_main.drawString(phrase, canvas_center_x, canvas_h - 23);
}

#define ROW_SIZE 40
#define PADDING 10

void drawMainMenu() {
  canvas_main.fillSprite(BLACK);
  canvas_main.setTextSize(2);
  canvas_main.setTextColor(GREEN);
  canvas_main.setColor(GREEN);
  canvas_main.setTextDatum(top_left);

  char display_str[50] = "";
  for (uint8_t i = 0; i < main_menu_len; i++) {
    sprintf(display_str, "%s %s", (menu_current_opt == i) ? ">" : " ",
            main_menu[i].name);
    int y = PADDING + (i * ROW_SIZE / 2);
    canvas_main.drawString(display_str, 0, y);
  }
}

void drawNearbyMenu() {
  canvas_main.clear(BLACK);
  canvas_main.setTextSize(2);
  canvas_main.setTextColor(GREEN);
  canvas_main.setColor(GREEN);
  canvas_main.setTextDatum(top_left);

  pwngrid_peer* pwngrid_peers = getPwngridPeers();
  uint8_t len = getPwngridRunTotalPeers();

  if (len == 0) {
    canvas_main.setTextColor(TFT_DARKGRAY);
    canvas_main.setCursor(0, PADDING);
    canvas_main.println("No nearby Pwnagotchis. Seriously?");
  }

  char display_str[50] = "";
  for (uint8_t i = 0; i < len; i++) {
    sprintf(display_str, "%s %s [%s]", (menu_current_opt == i) ? ">" : " ",
            pwngrid_peers[i].name, getRssiBars(pwngrid_peers[i].rssi));
    int y = PADDING + (i * ROW_SIZE / 2);
    canvas_main.drawString(display_str, 0, y);
  }
}

void drawSettingsMenu() {
  canvas_main.fillSprite(BLACK);
  canvas_main.setTextSize(2);
  canvas_main.setTextColor(GREEN);
  canvas_main.setColor(GREEN);
  canvas_main.setTextDatum(top_left);

  char display_str[50] = "";
  for (uint8_t i = 0; i < settings_menu_len; i++) {
    sprintf(display_str, "%s %s", (menu_current_opt == i) ? ">" : " ",
            settings_menu[i].name);
    int y = PADDING + (i * ROW_SIZE / 2);
    canvas_main.drawString(display_str, 0, y);
  }
}

void drawAboutMenu() {
  canvas_main.clear(BLACK);
  canvas_main.qrcode("https://github.com/viniciusbo/m5-palnagotchi",
                     (display_w / 2) - (display_h * 0.3), PADDING,
                     display_h * 0.65);
}

void drawMenu() {
  if (isNextPressed()) {
    // if (menu_current_opt < menu_current_size - 1) {
    menu_current_opt++;
    // } else {
    //   menu_current_opt = 0;
    // }
  }

  if (isPrevPressed()) {
    if (menu_current_opt > 0) {
      menu_current_opt--;
    }
  }

  // Change menu

  switch (menu_current_cmd) {
    case 0:
      if (isOkPressed()) {
        menu_current_cmd = main_menu[menu_current_opt].command;
        menu_current_opt = 0;
      }
      drawMainMenu();
      break;
    case 2:
      drawNearbyMenu();
      break;
    case 4:
      if (isOkPressed()) {
        menu_current_cmd = settings_menu[menu_current_opt].command;
        menu_current_opt = 0;
      }
      drawSettingsMenu();
      break;
    case 8:
      drawAboutMenu();
      break;
    default:
      drawMainMenu();
      break;
  }
}

