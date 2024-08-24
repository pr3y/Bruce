#if defined (CORE)
#include "Speaker.h"

SPEAKER::SPEAKER(void) {
    _volume = 8;
    _begun  = false;
}

void SPEAKER::begin() {
    _begun = true;
    ledcSetup(TONE_PIN_CHANNEL, 0, 13);
    ledcAttachPin(SPEAKER_PIN, TONE_PIN_CHANNEL);
    setBeep(1000, 100);
}

void SPEAKER::end() {
    mute();
    ledcDetachPin(SPEAKER_PIN);
    _begun = false;
}

void SPEAKER::tone(uint16_t frequency) {
    if (!_begun) begin();
    ledcWriteTone(TONE_PIN_CHANNEL, frequency);
    ledcWrite(TONE_PIN_CHANNEL, 0x400 >> _volume);
}

void SPEAKER::tone(uint16_t frequency, uint32_t duration) {
    tone(frequency);
    _count     = millis() + duration;
    speaker_on = 1;
}

void SPEAKER::beep() {
    if (!_begun) begin();
    tone(_beep_freq, _beep_duration);
}

void SPEAKER::setBeep(uint16_t frequency, uint16_t duration) {
    _beep_freq     = frequency;
    _beep_duration = duration;
}

void SPEAKER::setVolume(uint8_t volume) {
    _volume = 11 - volume;
}

void SPEAKER::mute() {
    ledcWriteTone(TONE_PIN_CHANNEL, 0);
    digitalWrite(SPEAKER_PIN, 0);
}

void SPEAKER::update() {
    if (speaker_on) {
        if (millis() > _count) {
            speaker_on = 0;
            mute();
        }
    }
}

void SPEAKER::write(uint8_t value) {
    dacWrite(SPEAKER_PIN, value);
}

void SPEAKER::playMusic(const uint8_t* music_data, uint16_t sample_rate) {
    uint32_t length         = strlen((char*)music_data);
    uint16_t delay_interval = ((uint32_t)1000000 / sample_rate);
    if (_volume != 11) {
        for (int i = 0; i < length; i++) {
            dacWrite(SPEAKER_PIN, music_data[i] / _volume);
            delayMicroseconds(delay_interval);
        }

        for (int t = music_data[length - 1] / _volume; t >= 0; t--) {
            dacWrite(SPEAKER_PIN, t);
            delay(2);
        }
    }
    // ledcSetup(TONE_PIN_CHANNEL, 0, 13);
    ledcAttachPin(SPEAKER_PIN, TONE_PIN_CHANNEL);
}
#endif