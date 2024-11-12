
#include "pn532ble.h"
#include "core/mykeyboard.h"
#include "core/display.h"
#include "core/scrollableTextArea.h"

Pn532ble::Pn532ble()
{
    setup();
}

Pn532ble::~Pn532ble()
{
}

void Pn532ble::setup()
{
    displayBanner();

    if (!connect())
        return;
    showDeviceInfo();

    delay(500);
    return loop();
}

bool Pn532ble::connect()
{
    displayInfo("Turn on PN532 BLE");
    delay(500);
    displayBanner();
    padprintln("");
    displayInfo("Searching...");

    if (!pn532_ble.searchForDevice())
    {
        displayError("Not found");
        delay(1000);
        return false;
    }

    if (!pn532_ble.connectToDevice())
    {
        displayError("Connect failed");
        delay(1000);
        return false;
    }
    displaySuccess("Connected");
    delay(800);

    return true;
}

void Pn532ble::loop()
{
    while (1)
    {
        if (checkEscPress())
        {
            returnToMenu = true;
            break;
        }

        if (checkSelPress())
        {
            selectMode();
        }
    }
}

void Pn532ble::selectMode()
{
    options = {
        {"Tag Scan", [&]()
         { setMode(HF_SCAN_MODE); }},
        {"Tag Dump", [&]()
         { setMode(HF_DUMP_MODE); }},
        {"Back", [&]()
         { setMode(GET_FW_MODE); }},
    };
    delay(200);
    loopOptions(options);
}

void Pn532ble::setMode(AppMode mode)
{
    currentMode = mode;

    displayBanner();
    switch (mode)
    {
    case GET_FW_MODE:
        showDeviceInfo();
        break;
    case HF_SCAN_MODE:
        hf14aScan();
        break;
    case HF_DUMP_MODE:
        hf14aDump();
        break;
    }
}

void Pn532ble::displayBanner()
{
    drawMainBorderWithTitle("PN532 BLE");
    padprintln("------------");
    delay(300);
}

void Pn532ble::showDeviceInfo()
{
    displayBanner();
    padprintln("Devices: " + String(pn532_ble.getName().c_str()));
    pn532_ble.setNormalMode();
    bool res = pn532_ble.getVersion();
    if (!res)
    {
        displayError("Get version failed");
        delay(1000);
        return;
    }
    uint8_t *version = pn532_ble.cmdResponse.data;
    uint8_t dataSize = pn532_ble.cmdResponse.dataSize;
    String versionStr = "Version: ";
    for (size_t i = 0; i < dataSize; i++)
    {
        versionStr += version[i] < 0x10 ? " 0" : " ";
        versionStr += String(version[i], HEX);
    }
    padprintln(versionStr);
    padprintln("------------");
    padprintln("");
    padprintln("Press OK to select mode.");
}

void Pn532ble::hf14aScan()
{
    displayBanner();
    padprintln("HF 14a Scan");
    delay(200);
    pn532_ble.setNormalMode();
    PN532_BLE::Iso14aTagInfo tagInfo = pn532_ble.hf14aScan();
    if (tagInfo.uid.empty())
    {
        displayError("No tag found");
    }
    else
    {
        padprintln("------------");
        padprintln("Type: " + tagInfo.type);
        padprintln("UID:  " + tagInfo.uid_hex);
        padprintln("ATQA: " + tagInfo.atqa_hex);
        padprintln("SAK:  " + tagInfo.sak_hex);
        bool isGen1A = pn532_ble.isGen1A();
        padprintln("Gen1A: " + String(isGen1A ? "Yes" : "No"));
        bool isGen3 = pn532_ble.isGen3();
        padprintln("Gen3:  " + String(isGen3 ? "Yes" : "No"));
        bool isGen4 = pn532_ble.isGen4("00000000");
        padprintln("Gen4:  " + String(isGen4 ? "Yes" : "No"));
    }
}

void updateArea(ScrollableTextArea &area)
{
    if (checkPrevPress())
    {
        area.scrollUp();
    }
    else if (checkNextPress())
    {
        area.scrollDown();
    }
    area.draw();
}

void Pn532ble::hf14aDump()
{
    displayBanner();
    padprintln("HF 14A Dump");
    pn532_ble.setNormalMode();
    PN532_BLE::Iso14aTagInfo tagInfo = pn532_ble.hf14aScan();
    padprintln("------------");
    if(tagInfo.uid.empty())
    {
        displayError("No tag found");
        return;
    }
    padprintln("UID:  " + tagInfo.uid_hex);

    ScrollableTextArea area(FP, 10, 28, WIDTH - 20, HEIGHT - 38);

    if(tagInfo.sak == 0x08 || tagInfo.sak == 0x09 || tagInfo.sak == 0x18){
        padprintln("Type: " + tagInfo.type);
        if (pn532_ble.isGen1A())
        {
            padprintln("Gen1A: Yes");
            padprintln("------------");
            delay(200);
            for (uint8_t i = 0; i < 64; i++)
            {
                uint8_t blockData[16];
                std::vector<uint8_t> res = pn532_ble.sendData({0x30, i}, true);
                if (res.size() < 18)
                {
                    displayError("Read failed");
                    return;
                }
                String blockStr = String(i) + " ";
                for (uint8_t j = 0; j < 16; j++)
                {
                    blockData[j] = res[j + 1];
                }

                for (uint8_t j = 0; j < 16; j++)
                {
                    blockStr += blockData[j] < 0x10 ? "0" : "";
                    blockStr += String(blockData[j], HEX);
                }
                area.addLine(blockStr);
                area.scrollDown();
                area.draw();
            }
            area.addLine("------------");
            area.scrollDown();
            area.draw();
        }
        else if (pn532_ble.isGen4("00000000"))
        {
            padprintln("Gen4: Yes");
            padprintln("------------");
            delay(200);
            for (uint8_t i = 0; i < 64; i++)
            {
                uint8_t blockData[16];
                std::vector<uint8_t> res = pn532_ble.sendData({0xCF, 0x00, 0x00, 0x00, 0x00, 0xCE, i}, true);
                if (res.size() < 18)
                {
                    displayError("Read failed");
                    return;
                }
                String blockStr = String(i) + " ";
                for (uint8_t j = 0; j < 16; j++)
                {
                    blockData[j] = res[j + 1];
                }

                for (uint8_t j = 0; j < 16; j++)
                {
                    blockStr += blockData[j] < 0x10 ? "0" : "";
                    blockStr += String(blockData[j], HEX);
                }
                area.addLine(blockStr);
                area.scrollDown();
                area.draw();
            }
        }
        else{
            tagInfo = pn532_ble.hf14aScan();
            padprintln("Found Mifare Classic");
            padprintln("------------");
            padprintln("Checking keys...");
            delay(200);
            std::vector<uint8_t> keys = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            for (uint8_t s = 0; s < 16; s++)
            {
                uint8_t blockIdex = s * 4;
                bool authResult = pn532_ble.mfAuth(tagInfo.uid, blockIdex, keys.data(), true);
                if (!authResult)
                {
                    authResult = pn532_ble.mfAuth(tagInfo.uid, blockIdex, keys.data(), false);
                }
                if (!authResult)
                {
                    displayError("Sector " + String(s) + " auth failed");
                    continue;
                }
                for (uint8_t i = 0; i < 4; i++)
                {
                    uint8_t blockData[16];
                    std::vector<uint8_t> res = pn532_ble.sendData({0x30, blockIdex + i}, true);
                    if (res.size() < 18)
                    {
                        displayError("Read failed");
                        return;
                    }
                    String blockStr = String(blockIdex + i) + " ";
                    for (uint8_t j = 0; j < 16; j++)
                    {
                        blockData[j] = res[j + 1];
                    }

                    for (uint8_t j = 0; j < 16; j++)
                    {
                        blockStr += blockData[j] < 0x10 ? "0" : "";
                        blockStr += String(blockData[j], HEX);
                    }
                    area.addLine(blockStr);
                    area.scrollDown();
                    area.draw();
                }

                area.scrollDown();
                area.draw();
            }
        }
    }

    while (checkSelPress())
    {
        updateArea(area);
        yield();
    }
    while (!checkSelPress())
    {
        updateArea(area);
        yield();
    }
}