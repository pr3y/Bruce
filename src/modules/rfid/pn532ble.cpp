
#include "pn532ble.h"
#include "core/mykeyboard.h"
#include "core/display.h"
#include "core/scrollableTextArea.h"
#include "core/sd_functions.h"

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

    if (connect())
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
        {"Load Dump", [&]()
         { setMode(HF_LOAD_DUMP_MODE); }}};
    if (pn532_ble.isConnected())
    {
        options.push_back({"Tag Scan", [=]()
                           { setMode(HF_SCAN_MODE); }});
        options.push_back({"Tag Dump", [=]()
                           { setMode(HF_DUMP_MODE); }});
    }
    options.push_back({"Back", [=]()
                       { setMode(GET_FW_MODE); }});

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
        if (pn532_ble.isConnected())
        {
            showDeviceInfo();
        }
        else
        {
            padprintln("Device not connected");
        }
        break;
    case HF_SCAN_MODE:
        hf14aScan();
        break;
    case HF_DUMP_MODE:
        hf14aDump();
        break;
    case HF_LOAD_DUMP_MODE:
        loadMifareClassicDumpFile();
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
    if (tagInfo.uid.empty())
    {
        displayError("No tag found");
        return;
    }
    dump.clear();
    padprintln("UID:  " + tagInfo.uid_hex);

    ScrollableTextArea area(FP, 10, 28, WIDTH - 20, HEIGHT - 38);

    if (tagInfo.sak == 0x08 || tagInfo.sak == 0x09 || tagInfo.sak == 0x18)
    {
        padprintln("Type: " + tagInfo.type);
        if (pn532_ble.isGen1A())
        {
            padprintln("Gen1A: Yes");
            padprintln("------------");
            delay(200);
            area.addLine("TYPE: " + tagInfo.type);
            area.scrollDown();
            area.draw();
            area.addLine("UID:  " + tagInfo.uid_hex);
            area.scrollDown();
            area.draw();
            area.addLine("MAGI: Gen1A");
            area.scrollDown();
            area.draw();
            area.addLine("------------");
            area.scrollDown();
            area.draw();
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
                    dump.push_back(blockData[j]);
                }

                for (uint8_t j = 0; j < 16; j++)
                {
                    blockStr += blockData[j] < 0x10 ? "0" : "";
                    blockStr += String(blockData[j], HEX);
                }
                area.addLine(blockStr);
                area.scrollDown();
                area.draw();
                if (dump.size() == 1024)
                {
                    if (saveMifareClassicDumpFile(dump, tagInfo.uid_hex))
                    {
                        displaySuccess("Dump saved");
                    }
                    else
                    {
                        displayError("Dump save failed");
                    }
                    dump.clear();
                }
                else
                {
                    displayError("Size invalid: " + String(dump.size()));
                }
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
            area.addLine("TYPE: " + tagInfo.type);
            area.scrollDown();
            area.draw();
            area.addLine("UID:  " + tagInfo.uid_hex);
            area.scrollDown();
            area.draw();
            area.addLine("MAGI: Gen4");
            area.scrollDown();
            area.draw();
            area.addLine("------------");
            area.scrollDown();
            area.draw();
            std::vector<uint8_t> dump;
            uint8_t sectorCount = getMifareClassicSectorCount(tagInfo.sak);
            for (uint8_t s = 0; s < sectorCount; s++)
            {
                uint8_t sectorBlockIdex = (s < 32) ? s * 4 : 32 * 4 + (s - 32) * 16;
                uint8_t sectorBlockSize = (s < 32) ? 4 : 16;
                for (uint8_t i = 0; i < sectorBlockSize; i++)
                {
                    uint8_t blockIndex = sectorBlockIdex + i;
                    uint8_t blockData[16];
                    std::vector<uint8_t> res = pn532_ble.sendData({0xCF, 0x00, 0x00, 0x00, 0x00, 0xCE, blockIndex}, true);
                    if (res.size() < 18)
                    {
                        displayError("Read failed");
                        return;
                    }
                    String blockStr = String(blockIndex) + " ";
                    for (uint8_t j = 0; j < 16; j++)
                    {
                        blockData[j] = res[j + 1];
                        dump.push_back(blockData[j]);
                        blockStr += blockData[j] < 0x10 ? "0" : "";
                        blockStr += String(blockData[j], HEX);
                    }
                    area.addLine(blockStr);
                    area.scrollDown();
                    area.draw();
                }
            }
            area.addLine("------------");
            area.scrollDown();
            area.draw();
            if (dump.size() == 320 || dump.size() == 1024 || dump.size() == 4096)
            {
                if (saveMifareClassicDumpFile(dump, tagInfo.uid_hex))
                {
                    displaySuccess("Dump saved");
                }
                else
                {
                    displayError("Dump save failed");
                }
            }
            else
            {
                displayError("Size invalid: " + String(dump.size()));
            }
        }
        else
        {
            tagInfo = pn532_ble.hf14aScan();
            padprintln("Found Mifare Classic");
            padprintln("------------");
            padprintln("Checking keys...");
            delay(200);
            area.addLine("TYPE: " + tagInfo.type);
            area.scrollDown();
            area.draw();
            area.addLine("UID:  " + tagInfo.uid_hex);
            area.scrollDown();
            area.draw();
            area.addLine("------------");
            area.scrollDown();
            area.draw();
            std::vector<uint8_t> dump;
            std::vector<uint8_t> keys = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            uint8_t sectorCount = getMifareClassicSectorCount(tagInfo.sak);
            for (uint8_t s = 0; s < sectorCount; s++)
            {
                pn532_ble.hf14aScan();
                uint8_t sectorBlockIdex = (s < 32) ? s * 4 : 32 * 4 + (s - 32) * 16;
                bool useKeyA = true;
                bool authResult = pn532_ble.mfAuth(tagInfo.uid, sectorBlockIdex, keys.data(), useKeyA);
                if (!authResult)
                {
                    useKeyA = false;
                    pn532_ble.hf14aScan();
                    authResult = pn532_ble.mfAuth(tagInfo.uid, sectorBlockIdex, keys.data(), useKeyA);
                }
                if (!authResult)
                {
                    displayError("Sector " + String(s) + " auth failed");
                    continue;
                }
                uint8_t sectorBlockSize = (s < 32) ? 4 : 16;
                for (uint8_t i = 0; i < sectorBlockSize; i++)
                {
                    uint8_t blockIndex = sectorBlockIdex + i;
                    uint8_t blockData[16];
                    std::vector<uint8_t> res = pn532_ble.mfRdbl(blockIndex);
                    if (res.size() < 16)
                    {
                        area.addLine("Sector " + String(s) + " Block " + String(blockIndex) + " read failed");
                        area.scrollDown();
                        area.draw();
                        continue;
                    }
                    String blockStr = String(blockIndex) + " ";
                    for (uint8_t j = 0; j < 16; j++)
                    {
                        blockData[j] = res[j + 1];
                        dump.push_back(blockData[j]);
                    }

                    if (i == sectorBlockSize - 1)
                    {
                        if (useKeyA)
                        {
                            for (uint8_t j = 0; j < 6; j++)
                            {
                                blockData[j] = keys[j];
                            }
                        }
                        else
                        {
                            for (uint8_t j = 0; j < 6; j++)
                            {
                                blockData[j + 10] = keys[j];
                            }
                        }
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
            area.addLine("------------");
            area.scrollDown();
            area.draw();

            if (dump.size() == 320 || dump.size() == 1024 || dump.size() == 4096)
            {
                if (saveMifareClassicDumpFile(dump, tagInfo.uid_hex))
                {
                    displaySuccess("Dump saved");
                }
                else
                {
                    displayError("Dump save failed");
                }
            }
            else
            {
                displayError("Dump size invalid: " + String(dump.size()));
            }
        }
        pn532_ble.wakeup();
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

uint8_t Pn532ble::getMifareClassicSectorCount(uint8_t sak)
{
    switch (sak)
    {
    case 0x08:
        return 16;
    case 0x09:
        return 5;
    case 0x18:
        return 40;
    default:
        return 0;
    }
}

void Pn532ble::loadMifareClassicDumpFile()
{
    FS *fs;
    if (!getFsStorage(fs))
        {
            padprintln("No storage found");
            return;
        }
    String filePath = loopSD(*fs, true, "bin");
    if (filePath == "")
    {
        padprintln("No file selected");
        return;
    }

    File file = (*fs).open(filePath, FILE_READ);
    if (!file)
    {
        padprintln("File open failed");
        return;
    }
    dump.clear();
    while (file.available())
    {
        dump.push_back(file.read());
    }
    file.close();
    // check dump size if is 320, 1024 or 4096
    if (dump.size() != 320 && dump.size() != 1024 && dump.size() != 4096)
    {
        padprintln("Invalid dump size: " + String(dump.size()));
        return;
    }

    displayBanner();

    ScrollableTextArea area(FP, 10, 28, WIDTH - 20, HEIGHT - 38);
    padprintln("Dump loaded");
    padprintln("------------");
    delay(200);
    uint8_t blockIndex = 0;
    for (size_t i = 0; i < dump.size(); i += 16)
    {
        String line = String(blockIndex) + " ";
        for (size_t j = 0; j < 16; j++)
        {
            line += dump[i + j] < 0x10 ? "0" : "";
            line += String(dump[i + j], HEX);
        }
        area.addLine(line);
        area.scrollDown();
        area.draw();
        blockIndex++;
    }
    area.addLine("------------");
    area.scrollDown();
    area.draw();

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

bool Pn532ble::saveMifareClassicDumpFile(std::vector<uint8_t> data, String uid)
{
    FS *fs;
    if (!getFsStorage(fs))
        return false;
    if (!(*fs).exists("/rfid"))
        (*fs).mkdir("/rfid");
    if (!(*fs).exists("/rfid/mf"))
        (*fs).mkdir("/rfid/mf");
    String fileName = "hf-mf-" + uid;
    if ((*fs).exists("/rfid/mf/" + fileName + ".bin"))
    {
        int i = 1;
        fileName += "_";
        while ((*fs).exists("/rfid/mf/" + fileName + String(i) + ".bin"))
            i++;
        fileName += String(i);
    }
    File file = (*fs).open("/rfid/mf/" + fileName + ".bin", FILE_WRITE);
    if (!file)
    {
        return false;
    }
    for (size_t i = 0; i < data.size(); i++)
    {
        file.write(data[i]);
    }
    file.close();
    return true;
}