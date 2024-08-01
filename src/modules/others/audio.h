#pragma once
#include <SPIFFS.h>
#include <ESP8266Audio.h>
#include <ESP8266SAM.h>

bool playAudio(FS fs, String filepath);