#include "audio.h"

bool playAudio(FS fs, String filepath) {
    AudioFileSource* source = new AudioFileSourceFS(fs, filepath.c_str());
    AudioGenerator* generator = NULL;

    AudioOutputI2S* audioout =
        new AudioOutputI2S();  // https://github.com/earlephilhower/ESP8266Audio/blob/master/src/AudioOutputI2S.cpp#L32
#ifdef CARDPUTER
    audioout->SetPinout(41, 43, 42);  // bclk, wclk, dout
#endif

    if (filepath.endsWith(".txt") || filepath.endsWith(".rtttl"))
        generator = new AudioGeneratorRTTTL();
    if (filepath.endsWith(".wav")) 
      generator = new AudioGeneratorWAV();
    if (filepath.endsWith(".mod")) 
      generator = new AudioGeneratorMOD();
    if (filepath.endsWith(".mp3")) {
        generator = new AudioGeneratorMP3();
        source = new AudioFileSourceID3(source);
    }
    if (filepath.endsWith(".opus")) generator = new AudioGeneratorOpus();

    if (generator && source && audioout) {
        Serial.println("Start audio");
        generator->begin(source, audioout);
        // TODO async play
        while (generator->isRunning()) {
            if (!generator->loop()) generator->stop();
        }
        audioout->stop();
        source->close();
        Serial.println("Stop audio");

        delete generator;
        delete source;
        delete audioout;
        
    }
    return true;
}