#include "WhoisMenu.h"
#include "../display.h"
#include "../settings.h"
#include "../utils.h"

void WhoisMenu::displayTextScrollable(const char* text) {
    ScrollableTextArea area(FP, 10, 28, tftWidth - 20, tftHeight - 38);
    area.fromString(text);
    area.show(true);
}

void WhoisMenu::consultarWhois(const String& domain) {
    if (WiFi.status() != WL_CONNECTED) {
        displayError("WiFi não conectado");
        return;
    }

    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextColor(bruceConfig.priColor);
    tft.drawString("Consultando WHOIS...", 10, tftHeight/2);

    HTTPClient http;
    String url = "https://api.apilayer.com/whois/check?domain=" + domain;

    http.begin(url);
    http.addHeader("apikey", bruceConfig.whoisApiKey);

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
        String response = http.getString();
        displayTextScrollable(response.c_str());
    } else {
        displayError("Erro ao consultar WHOIS");
    }

    http.end();
}

void WhoisMenu::consultarWhoisDetalhado(const String& domain) {
    if (WiFi.status() != WL_CONNECTED) {
        displayError("WiFi não conectado");
        return;
    }

    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextColor(bruceConfig.priColor);
    tft.drawString("Consultando WHOIS detalhado...", 10, tftHeight/2);

    HTTPClient http;
    String url = "https://api.apilayer.com/whois/query?domain=" + domain;

    http.begin(url);
    http.addHeader("apikey", bruceConfig.whoisApiKey);

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
        String response = http.getString();
        // Apenas mostra a resposta JSON pura
        displayTextScrollable(response.c_str());
    } else {
        displayError("Erro ao consultar WHOIS");
    }

    http.end();
}

void WhoisMenu::optionsMenu() {
    options = {
        {"Verificar Disponibilidade", [=]() {
            String domain = ::keyboard("", 100, "Digite o domínio:");
            if (domain.length() > 0) {
                consultarWhois(domain);
            }
        }},
        {"Consulta Detalhada", [=]() {
            String domain = ::keyboard("", 100, "Digite o domínio:");
            if (domain.length() > 0) {
                consultarWhoisDetalhado(domain);
            }
        }},
        {"Configurar API Key", [=]() {
            String apiKey = ::keyboard(bruceConfig.whoisApiKey, 100, "API Key do WHOIS:");
            if (apiKey.length() > 0) {
                bruceConfig.setWhoisApiKey(apiKey);
            }
        }}
    };
    addOptionToMainMenu();
    loopOptions(options, true, "WHOIS");
}

void WhoisMenu::drawIcon(float scale) {
    clearIconArea();
    int iconW = scale * 32;
    int iconH = scale * 32;

    if (iconW % 2 != 0) iconW++;
    if (iconH % 2 != 0) iconH++;

    int iconX = iconCenterX - iconW / 2;
    int iconY = iconCenterY - iconH / 2;

    tft.fillRoundRect(iconX, iconY, iconW, iconH, 5, bruceConfig.priColor);
    tft.drawString("W", iconX + iconW/4, iconY + iconH/4);
}

void WhoisMenu::drawIconImg() {
    drawIcon(1.0);
}
