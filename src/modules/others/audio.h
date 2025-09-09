
#include <SPIFFS.h>
// Keep SPIFFS first

bool playAudioFile(FS *fs, String filepath); // TODO: bool async arg -> play in a task?

bool playAudioRTTTLString(String song);

bool tts(String text);

bool isAudioFile(String filePath);

void playTone(unsigned int frequency, unsigned long duration = 0UL, short waveType = 0);

void _tone(unsigned int frequency, unsigned long duration = 0UL);
