
#include <SPIFFS.h>
#include <ESP8266Audio.h>
#include <ESP8266SAM.h>

bool playAudioFile(FS* fs, String filepath);  // TODO: bool async arg -> play in a task?

bool playAudioRTTTLString(String song);
 
bool tts(String text);