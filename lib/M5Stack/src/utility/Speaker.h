#if defined (CORE)
#ifndef _SPEAKER_H_
#define _SPEAKER_H_

#include "Arduino.h"
#include "Config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "esp32-hal-dac.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

class SPEAKER {
   public:
    SPEAKER(void);

    void begin();
    void end();
    void mute();
    void tone(uint16_t frequency);
    void tone(uint16_t frequency, uint32_t duration);
    void beep();
    void setBeep(uint16_t frequency, uint16_t duration);
    void update();

    void write(uint8_t value);
    void setVolume(uint8_t volume);
    void playMusic(const uint8_t *music_data, uint16_t sample_rate);

   private:
    uint32_t _count;
    uint8_t _volume;
    uint16_t _beep_duration;
    uint16_t _beep_freq;
    bool _begun;
    bool speaker_on;
};
#endif
#endif