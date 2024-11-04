#include "ble_common.h"
#include "core/mykeyboard.h"

#define SERVICE_UUID "1bc68b2a-f3e3-11e9-81b4-2a2ae2dbcce4"
#define CHARACTERISTIC_RX_UUID "1bc68da0-f3e3-11e9-81b4-2a2ae2dbcce4"
#define CHARACTERISTIC_TX_UUID "1bc68efe-f3e3-11e9-81b4-2a2ae2dbcce4"

#define SCANTIME 5
#define SCANTYPE ACTIVE
#define SCAN_INT 100
#define SCAN_WINDOW 99

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
    NimBLEAttValue data;
    void onWrite(NimBLECharacteristic *pCharacteristic)
    {
        data = pCharacteristic->getValue();
    }
};

int scanTime = SCANTIME; //In seconds
BLEScan *pBLEScan;

uint8_t sta_mac[6];
char strID[18];
char strAddl[200];

void ble_info(String name, String address, String signal)
{
    drawMainBorder();
    tft.setTextColor(bruceConfig.priColor);
    tft.drawCentreString("-=Information=-", WIDTH/2, 28,SMOOTH_FONT);
    tft.drawString("Name: " + name, 10, 48);
    tft.drawString("Adresse: " + address, 10, 66);
    tft.drawString("Signal: " + String(signal) + " dBm", 10, 84);
    tft.drawCentreString("   Press " + String(BTN_ALIAS) + " to act",WIDTH/2,HEIGHT-20,1);

    delay(300);
    while(!checkSelPress()) {
        while(!checkSelPress()) { yield(); } // timerless debounce
        returnToMenu=true;
        break;
    }
}

class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        String bt_title;
        String bt_name;
        String bt_address;
        String bt_signal;

        bt_name = advertisedDevice->getName().c_str();
        bt_title = advertisedDevice->getName().c_str();
        bt_address = advertisedDevice->getAddress().toString().c_str();
        bt_signal = String(advertisedDevice->getRSSI());
        //Serial.println("\n\nAddress - " + bt_address + "Name-"+ bt_name +"\n\n");
        if(bt_title.isEmpty()) bt_title = bt_address;
        if(bt_name.isEmpty()) bt_name="<no name>";
        // If BT name is empty, set NONAME
        if(ESP.getFreeHeap()>4096) options.push_back({bt_title.c_str(), [=]() { ble_info(bt_name, bt_address, bt_signal); }});
        else {
            Serial.println("Memory low, stopping BLE scan...");
            pBLEScan->stop();
        }
    }
};

void ble_scan_setup()
{
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    // Active scan uses more power, but get results faster
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(SCAN_INT);
    // Less or equal setInterval value
    pBLEScan->setWindow(SCAN_WINDOW);

    // Bluetooth MAC Address
    esp_read_mac(sta_mac,ESP_MAC_BT);
    sprintf(strID,"%02X:%02X:%02X:%02X:%02X:%02X",sta_mac[0],sta_mac[1],sta_mac[2],sta_mac[3],sta_mac[4],sta_mac[5]);
    delay(500);
}

void ble_scan()
{
    displayRedStripe("Scanning..", TFT_WHITE, bruceConfig.priColor);

    options = { };
    ble_scan_setup();
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);

    options.push_back({"Main menu", [=]() { backToMenu(); }});

    delay(200);
    loopOptions(options);
    delay(200);

    // Delete results fromBLEScan buffer to release memory
    pBLEScan->clearResults();
}

bool initBLEServer()
{
    uint64_t chipid = ESP.getEfuseMac();
    String blename = "Bruce-" + String((uint8_t)(chipid >> 32), HEX);

    BLEDevice::init(blename.c_str());
    // BLEDevice::setPower(ESP_PWR_LVL_N12);
    pServer = BLEDevice::createServer();

    pServer->setCallbacks(new MyServerCallbacks());
    pService = pServer->createService(SERVICE_UUID);
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_RX_UUID, NIMBLE_PROPERTY::NOTIFY);

    pTxCharacteristic->addDescriptor(new NimBLE2904());
    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_TX_UUID, NIMBLE_PROPERTY::WRITE);
    pRxCharacteristic->setCallbacks(new MyCallbacks());

    return true;
}

void disPlayBLESend()
{
    uint8_t senddata[2] = {0};
    tft.fillScreen(TFT_BLACK);
    drawMainBorder(); // Moved up to avoid drawing screen issues
    tft.setTextSize(1);

    pService->start();
    pServer->getAdvertising()->start();

    uint64_t chipid = ESP.getEfuseMac();
    String blename = "Bruce-" + String((uint8_t)(chipid >> 32), HEX);

    BLEConnected=true;

    bool wasConnected = false;
    bool first_run = true;
    while (!checkEscPress())
    {
        if (deviceConnected)
        {
            if (!wasConnected) {
                tft.fillRect(10, 26, WIDTH-20, HEIGHT-36, TFT_BLACK);
                drawBLE_beacon(180, 28, TFT_BLUE);
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                tft.setTextSize(FM);
                tft.setCursor(12, 50);
                // tft.printf("BLE connect!\n");
                tft.printf("BLE Send\n");
                tft.setTextSize(FM);
            }
            tft.fillRect(10, 100, WIDTH-20, 28, TFT_BLACK);
            tft.setCursor(12, 100);
            if (senddata[0] % 4 == 0)
            {
                tft.printf("0x%02X>    ", senddata[0]);
            }
            else if (senddata[0] % 4 == 1)
            {
                tft.printf("0x%02X>>   ", senddata[0]);
            }
            else if (senddata[0] % 4 == 2)
            {
                tft.printf("0x%02X >>  ", senddata[0]);
            }
            else if (senddata[0] % 4 == 3)
            {
                tft.printf("0x%02X  >  ", senddata[0]);
            }

            senddata[1]++;
            if (senddata[1] > 3)
            {
                senddata[1] = 0;
                senddata[0]++;
                pTxCharacteristic->setValue(senddata, 1);
                pTxCharacteristic->notify();
            }
            wasConnected = true;
        }
        else
        {
            if (wasConnected or first_run) {
                first_run = false;
                tft.fillRect(10, 26, WIDTH-20, HEIGHT-36, TFT_BLACK);
                tft.setTextSize(2);
                tft.setCursor(12, 50);
                tft.setTextColor(TFT_RED);
                tft.printf("BLE disconnect\n");
                tft.setCursor(12, 75);
                tft.setTextColor(tft.color565(18, 150, 219));

                tft.printf(String("Name:" + blename + "\n").c_str());
                tft.setCursor(12, 100);
                tft.printf("UUID:1bc68b2a\n");
                drawBLE_beacon(180, 40, TFT_DARKGREY);
            }
            wasConnected = false;
        }
        delay(200);
    }

    tft.setTextColor(TFT_WHITE);
    pService->~NimBLEService();
    pServer->getAdvertising()->stop();
    BLEDevice::deinit();
    BLEConnected=false;
}

static bool is_ble_inited = false;

void ble_test()
{
    printf("ble test\n");

    // if (!is_ble_inited)
    // {
    printf("Init ble server\n");
    initBLEServer();
    delay(100);
    is_ble_inited = true;
    // }

    disPlayBLESend();

    printf("Quit ble test\n");
}

