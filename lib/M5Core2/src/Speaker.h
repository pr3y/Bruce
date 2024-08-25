#if defined (CORE2)
#ifndef _SPEAKER_H_
#define _SPEAKER_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2s.h>
#include <esp_err.h>
#include "AXP.h"
#include "Arduino.h"

#define CONFIG_I2S_BCK_PIN     12  // 定义I2S相关端口
#define CONFIG_I2S_LRCK_PIN    0
#define CONFIG_I2S_DATA_PIN    2
#define CONFIG_I2S_DATA_IN_PIN 34

#define Speak_I2S_NUMBER I2S_NUM_0  // 定义扬声器端口

#define MODE_MIC  0  // 定义工作模式
#define MODE_SPK  1
#define DATA_SIZE 1024

enum pmic_t { pmic_unknown = 0, pmic_axp192, pmic_axp2101 };

class Speaker {
   public:
    void begin(void);
    bool InitI2SSpeakOrMic(int mode);
    // Plays the given amount of bytes from the given data array and returns the
    // amount of bytes, that were actually played by the speaker.
    size_t PlaySound(const unsigned char* data, const size_t& amount_of_bytes);

   private:
    pmic_t _pmic;
};
#endif
#endif