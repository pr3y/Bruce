
#include "pn532ble.h"
#include "core/mykeyboard.h"
#include "core/display.h"

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
    displayInfo("Turn on PN532 BLE device", true);

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
    std::string deviceName = pn532_ble.getName();
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
        {"Back", [&]() { setMode(GET_FW_MODE); }},
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
        pn532_ble.setNormalMode();
        hf14aScan();
        break;
    }
}

void Pn532ble::displayBanner()
{
    drawMainBorderWithTitle("PN532 BLE");
    padprintln("PN532 HSU with Bluetooth 4.0+");
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
    bool res = pn532_ble.hf14aScan();
    delay(20);
    if (!res)
    {
        displayError("No tag found");
    }
    else
    {
        u_int8_t *data = pn532_ble.cmdResponse.data;
        u_int8_t dataSize = pn532_ble.cmdResponse.dataSize;
        Iso14aTagInfo tagInfo = parseHf14aScan(data, dataSize);
        padprintln("------------");
        padprintln("UID:  " + tagInfo.uid_hex);
        padprintln("ATQA: " + tagInfo.atqa_hex);
        padprintln("SAK:  " + tagInfo.sak_hex);
        bool isGen1A = pn532_ble.isGen1A(); 
        padprintln("Gen1A: " + String(isGen1A ? "Yes" : "No"));
    }
}

String bytes2HexString(std::vector<uint8_t> *data, uint8_t dataSize)
{
    String hexString = "";
    for (size_t i = 0; i < dataSize; i++)
    {
        hexString += (*data)[i] < 0x10 ? " 0" : " ";
        hexString += String((*data)[i], HEX);
    }
    hexString.toUpperCase();
    return hexString;
}

Pn532ble::Iso14aTagInfo Pn532ble::parseHf14aScan(uint8_t *data, uint8_t dataSize)
{
    Iso14aTagInfo tagInfo;
    tagInfo.atqa = {data[2], data[3]};
    tagInfo.sak = data[4];
    tagInfo.uidSize = data[5];
    tagInfo.uid.assign(data + 6, data + 6 + tagInfo.uidSize);
    tagInfo.uid_hex = "";
    for (size_t i = 0; i < tagInfo.uid.size(); i++)
    {
        tagInfo.uid_hex += tagInfo.uid[i] < 0x10 ? " 0" : " ";
        tagInfo.uid_hex += String(tagInfo.uid[i], HEX);
    }
    tagInfo.uid_hex.toUpperCase();
    tagInfo.atqa_hex = bytes2HexString(&tagInfo.atqa, 2);
    std::vector<uint8_t> sakVector = {tagInfo.sak};
    tagInfo.sak_hex = bytes2HexString(&sakVector, 1);
    return tagInfo;
}