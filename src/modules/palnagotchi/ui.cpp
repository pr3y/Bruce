#ifndef LITE_VERSION
#include "ui.h"

#if defined(HAS_SCREEN)
    TFT_eSprite canvas_top(&tft);
    TFT_eSprite canvas_main(&tft);
    TFT_eSprite canvas_bot(&tft);
#else
    SerialDisplayClass canvas_top(&tft);
    SerialDisplayClass canvas_main(&tft);
    SerialDisplayClass canvas_bot(&tft);
#endif

int32_t display_w;
int32_t display_h;
int32_t canvas_h;
int32_t canvas_center_x;
int32_t canvas_top_h;
int32_t canvas_bot_h;
int32_t canvas_peers_menu_h;
int32_t canvas_peers_menu_w;

uint8_t menu_current_cmd = 0;
uint8_t menu_current_opt = 0;

void initUi() {
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(FGCOLOR, TFT_BLACK);

  display_w = WIDTH;
  display_h = HEIGHT;
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

void deInitUi() {
  canvas_top.deleteSprite();
  canvas_bot.deleteSprite();
  canvas_main.deleteSprite();
}

void updateUi(bool show_toolbars) {
  uint8_t mood_id = getCurrentMoodId();
  String mood_face = getCurrentMoodFace();
  String mood_phrase = getCurrentMoodPhrase();
  bool mood_broken = isCurrentMoodBroken();

  drawTopCanvas();
  drawBottomCanvas(getPwngridRunTotalPeers(), getPwngridTotalPeers(),
                   getPwngridLastFriendName(), getPwngridClosestRssi());

  drawMood(mood_face, mood_phrase, mood_broken);

  tft.startWrite();
  if (show_toolbars) {
    canvas_top.pushSprite(0, 0);
    canvas_bot.pushSprite(0, canvas_top_h + canvas_h);
  }
  canvas_main.pushSprite(0, canvas_top_h);
  tft.endWrite();
}

void drawTopCanvas() {
  canvas_top.fillSprite(TFT_BLACK);
  canvas_top.setTextSize(1);
  canvas_top.setTextColor(FGCOLOR, TFT_BLACK);
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
  canvas_top.drawLine(0, canvas_top_h - 1, display_w, canvas_top_h - 1, FGCOLOR);
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
  canvas_bot.fillSprite(TFT_BLACK);
  canvas_bot.setTextSize(1);
  canvas_bot.setTextColor(FGCOLOR, TFT_BLACK);
  canvas_bot.setTextDatum(top_left);

  String rssi_bars = getRssiBars(rssi);
  char stats[25] = "FRND 0 (0)";
  if (friends_run > 0) {
    sprintf(stats, "FRND %d (%d) [%s] %s", friends_run, friends_tot,
            last_friend_name, rssi_bars);
  }

  canvas_bot.drawString(stats, 0, 5);
  canvas_bot.setTextDatum(top_right);
  canvas_bot.drawString("NOT AI", display_w, 5);
  canvas_bot.drawLine(0, 0, display_w, 0, FGCOLOR);
}

void drawMood(String face, String phrase, bool broken) {
  if (broken == true) {
    canvas_main.setTextColor(RED);
  } else {
    canvas_main.setTextColor(FGCOLOR);
  }

  canvas_main.setTextSize(4);
  canvas_main.setTextDatum(middle_center);
  canvas_main.fillSprite(TFT_BLACK);
  canvas_main.drawCentreString(face, canvas_center_x, canvas_h / 3, SMOOTH_FONT);
  canvas_main.setTextDatum(bottom_center);
  canvas_main.setTextSize(1);
  canvas_main.drawCentreString(phrase, canvas_center_x, canvas_h - 30, SMOOTH_FONT);
}

#define ROW_SIZE 40
#define PADDING 10

void drawNearbyMenu() {
  canvas_main.fillScreen(BGCOLOR);
  canvas_main.setTextSize(2);
  canvas_main.setTextColor(FGCOLOR, TFT_BLACK);
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
#endif
