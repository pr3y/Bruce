#define FASTLED_JSON_GUARD
#include "ChatGPTMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "core/mykeyboard.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "core/scrollableTextArea.h"

const char* CHATGPT_API_ENDPOINT = "https://api.openai.com/v1/chat/completions";

void ChatGPTMenu::sendMessageToChatGPT(const String& message) {
    if (WiFi.status() != WL_CONNECTED) {
        displayError("WiFi não conectado");
        return;
    }

    HTTPClient http;
    http.begin(CHATGPT_API_ENDPOINT);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(bruceConfig.chatgptApiKey));

    StaticJsonDocument<2048> doc;
    doc["model"] = "gpt-3.5-turbo";

    JsonArray messages = doc["messages"].to<JsonArray>();
    JsonObject messageObj = messages.add<JsonObject>();
    messageObj["role"] = "user";
    messageObj["content"] = message;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
        String response = http.getString();

        StaticJsonDocument<8192> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, response);

        if (!error && responseDoc["choices"].is<JsonArray>()) {
            JsonArray choices = responseDoc["choices"].as<JsonArray>();
            if (choices.size() > 0 && choices[0]["message"]["content"].is<const char*>()) {
                String chatResponse = choices[0]["message"]["content"].as<String>();
                this->displayTextScrollable(chatResponse.c_str());
            } else {
                displayError("Resposta inesperada do ChatGPT");
            }
        } else {
            displayError("Erro ao processar resposta JSON");
        }
    } else {
        displayError("Erro na requisição");
    }

    http.end();
}

void ChatGPTMenu::displayTextScrollable(const char* text) {
    ScrollableTextArea area(FP, 10, 28, tftWidth - 20, tftHeight - 38);
    area.fromString(text);
    area.show(true);

    // Após mostrar a resposta e o usuário pressionar ENTER
    String message = ::keyboard("", 500, "Nova pergunta (vazio para sair):");
    if (message.length() > 0) {
        sendMessageToChatGPT(message);
    }
}

void ChatGPTMenu::initI2S() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = (i2s_bits_per_sample_t)SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config;
    pin_config.mck_io_num = I2S_PIN_NO_CHANGE;
    pin_config.bck_io_num = 41;
    pin_config.ws_io_num = 42;
    pin_config.data_in_num = 2;
    pin_config.data_out_num = I2S_PIN_NO_CHANGE;

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
}

void ChatGPTMenu::deinitI2S() {
    i2s_driver_uninstall(I2S_PORT);
}

String ChatGPTMenu::transcribeAudio(const uint8_t* audioData, size_t length) {
    HTTPClient http;
    http.begin("https://api.openai.com/v1/audio/transcriptions");
    http.addHeader("Authorization", "Bearer " + String(bruceConfig.chatgptApiKey));

    String boundary = "boundary";
    http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

    String body = "--" + boundary + "\r\n";
    body += "Content-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\n";
    body += "Content-Type: audio/wav\r\n\r\n";

    // Adiciona os dados do áudio
    for (size_t i = 0; i < length; i++) {
        body += (char)audioData[i];
    }

    body += "\r\n--" + boundary + "\r\n";
    body += "Content-Disposition: form-data; name=\"model\"\r\n\r\n";
    body += "whisper-1\r\n";
    body += "--" + boundary + "--\r\n";

    int httpResponseCode = http.POST(body);
    String response = "";

    if (httpResponseCode > 0) {
        response = http.getString();
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            response = doc["text"].as<String>();
        }
    }

    http.end();
    return response;
}

void ChatGPTMenu::captureAndSendVoiceMessage() {
    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextColor(bruceConfig.priColor);
    tft.drawString("Pressione ENTER para começar a gravar", 10, tftHeight/2 - 20);
    tft.drawString("e ENTER novamente para parar", 10, tftHeight/2);

    while (!KeyStroke.enter) {
        delay(10);
    }

    initI2S();

    const int bufferSize = 1024;
    uint8_t* audioBuffer = (uint8_t*)malloc(bufferSize * sizeof(uint8_t));
    std::vector<uint8_t> recordedAudio;

    tft.fillScreen(bruceConfig.bgColor);
    tft.drawString("Gravando... (ENTER para parar)", 10, tftHeight/2);

    while (!KeyStroke.enter) {
        size_t bytesRead = 0;
        i2s_read(I2S_PORT, audioBuffer, bufferSize, &bytesRead, portMAX_DELAY);
        if (bytesRead > 0) {
            recordedAudio.insert(recordedAudio.end(), audioBuffer, audioBuffer + bytesRead);
        }
        delay(10);
    }

    free(audioBuffer);
    deinitI2S();

    tft.fillScreen(bruceConfig.bgColor);
    tft.drawString("Transcrevendo...", 10, tftHeight/2);

    String transcription = transcribeAudio(recordedAudio.data(), recordedAudio.size());

    if (transcription.length() > 0) {
        sendMessageToChatGPT(transcription);
    } else {
        displayError("Erro na transcrição");
    }
}

void ChatGPTMenu::optionsMenu() {
    options = {
        {"Enviar Mensagem", [=]() {
            String message = ::keyboard("", 500, "Digite sua mensagem:");
            if (message.length() > 0) {
                sendMessageToChatGPT(message);
            }
        }},
        {"Enviar Mensagem por Voz", [=]() {
            captureAndSendVoiceMessage();
        }},
        {"Configurar API Key", [=]() {
            String apiKey = ::keyboard(bruceConfig.chatgptApiKey, 100, "API Key do ChatGPT:");
            if (apiKey.length() > 0) {
                bruceConfig.setChatGPTApiKey(apiKey);
            }
        }},
    };
    addOptionToMainMenu();
    loopOptions(options, true, "ChatGPT");
}

void ChatGPTMenu::drawIcon(float scale) {
    clearIconArea();
    int iconW = scale * 32;
    int iconH = scale * 32;

    if (iconW % 2 != 0) iconW++;
    if (iconH % 2 != 0) iconH++;

    int iconX = iconCenterX - iconW / 2;
    int iconY = iconCenterY - iconH / 2;

    tft.fillRoundRect(iconX, iconY, iconW, iconH, 5, bruceConfig.priColor);
    tft.fillTriangle(
        iconX + iconW / 4,
        iconY + iconH,
        iconX + iconW / 2,
        iconY + iconH + 10,
        iconX + iconW / 2,
        iconY + iconH,
        bruceConfig.priColor
    );
}

void ChatGPTMenu::drawIconImg() {
    drawIcon(1.0);
}
