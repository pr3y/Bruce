#ifndef __CHATGPT_MENU_H__
#define __CHATGPT_MENU_H__

#include <MenuItemInterface.h>
#include <driver/i2s.h>
#include "../display.h"
#include "../mykeyboard.h"

class ChatGPTMenu : public MenuItemInterface {
public:
    ChatGPTMenu() : MenuItemInterface("ChatGPT") {}

    void optionsMenu(void) override;
    void drawIcon(float scale) override;
    void drawIconImg() override;
    bool getTheme() override { return true; }
    void displayTextScrollable(const char* text);
    void sendMessageToChatGPT(const String& message);
    void captureAndSendVoiceMessage();

private:
    std::vector<Option> options;
    static constexpr i2s_port_t I2S_PORT = I2S_NUM_0;
    static constexpr int SAMPLE_RATE = 16000;
    static constexpr int SAMPLE_BITS = 16;
    static constexpr int CHANNELS = 1;

    void initI2S();
    void deinitI2S();
    String transcribeAudio(const uint8_t* audioData, size_t length);
};

#endif
