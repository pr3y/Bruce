#include "audio.h"
#include <ESP8266Audio.h>
#include <ESP8266SAM.h>
#include "core/mykeyboard.h"


#if defined(HAS_NS4168_SPKR)

bool playAudioFile(FS* fs, String filepath) {
	
    AudioOutputI2S* audioout = new AudioOutputI2S();  // https://github.com/earlephilhower/ESP8266Audio/blob/master/src/AudioOutputI2S.cpp#L32
	audioout->SetPinout(BCLK, WCLK, DOUT);
	
    AudioFileSource* source = new AudioFileSourceFS(*fs, filepath.c_str());
    if(!source) return false;
    
	AudioGenerator* generator = NULL;

	// switch on extension
    filepath.toLowerCase(); // case-insensitive match
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
	/* 2FIX: compilation issues
	if(song.endsWith(".mid"))  {
	  // need to load a soundfont
	  AudioFileSource* sf2 = NULL;
	  if(setupSdCard()) sf2 = new AudioFileSourceSD("1mgm.sf2");  // TODO: make configurable
	  if(!sf2) sf2 = new AudioFileSourceLittleFS("1mgm.sf2");  // TODO: make configurable
	  if(sf2) {
		// a soundfount was found
		AudioGeneratorMIDI* midi = new AudioGeneratorMIDI();
		generator->SetSoundfont(sf2);
		generator = midi;
	  }
	}*/
            
    if (generator && source && audioout) {
        Serial.println("Start audio");
        generator->begin(source, audioout);
        while (generator->isRunning()) {
            if (!generator->loop()) generator->stop(); // || checkEscPress()
            
        }
        audioout->stop();
        source->close();
        Serial.println("Stop audio");

        delete generator;
        delete source;
        delete audioout;
        
        return true;
    }
    
    return false;  // init error
}

bool playAudioRTTTLString(String song) {
	// derived from https://github.com/earlephilhower/ESP8266Audio/blob/master/examples/PlayRTTTLToI2SDAC/PlayRTTTLToI2SDAC.ino
	
	song.trim();
	if(song=="") return false;
	
    AudioOutputI2S* audioout = new AudioOutputI2S();
	audioout->SetPinout(BCLK, WCLK, DOUT);
	
	AudioGenerator* generator = new AudioGeneratorRTTTL();
	
	AudioFileSource* source = new AudioFileSourcePROGMEM( song.c_str(), song.length() );
    
    if (generator && source && audioout) {
        Serial.println("Start audio");
        generator->begin(source, audioout);
        while (generator->isRunning()) {
            if (!generator->loop()) generator->stop();
        }
        audioout->stop();
        source->close();
        Serial.println("Stop audio");

        delete generator;
        delete source;
        delete audioout;
        
        return true;
    }
    return false;    // init error
}

bool tts(String text){
	text.trim();
	if(text=="") return false;
	
    AudioOutputI2S* audioout = new AudioOutputI2S();
	audioout->SetPinout(BCLK, WCLK, DOUT);
	
	// https://github.com/earlephilhower/ESP8266SAM/blob/master/examples/Speak/Speak.ino
	audioout->begin();
	ESP8266SAM *sam = new ESP8266SAM;
	sam->Say(audioout, text.c_str());
	delete sam;
	return true;
}


bool isAudioFile(String filepath) {
    
    return filepath.endsWith(".txt") || filepath.endsWith(".rtttl") || 
        filepath.endsWith(".wav") || filepath.endsWith(".mod") || filepath.endsWith(".mp3") ;
}
#endif
