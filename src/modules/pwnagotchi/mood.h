/*
Thanks to thoses developers for their projects:
* @7h30th3r0n3 : https://github.com/7h30th3r0n3/Evil-M5Core2 and https://github.com/7h30th3r0n3/PwnGridSpam
* @viniciusbo : https://github.com/viniciusbo/m5-palnagotchi
* @sduenasg : https://github.com/sduenasg/pio_palnagotchi

Thanks to @bmorcelli for his help doing a better code.
*/

#include <Arduino.h>

#define MOOD_BROKEN 19

void setMood(uint8_t mood, String face = "", String phrase = "", bool broken = false);
uint8_t getCurrentMoodId();
int getNumberOfMoods();
String getCurrentMoodFace();
String getCurrentMoodPhrase();
bool isCurrentMoodBroken();
