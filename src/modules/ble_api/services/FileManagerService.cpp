#include "FileManagerService.h"
#include "globals.h"
#include <BLEDevice.h>
#include <BLEUtils.h>

FileManagerService::FileManagerService() = default;

FileManagerService::~FileManagerService() = default;

typedef struct taskParam {
    char *filename;
    NimBLECharacteristic *characteristic;
} taskParam;

/*
 * Error code in BLE Characteristic:
 * -1: SD not installed
 * -2: File not found
 * -3: Can't open the file
 * -4: Invalid FS
 */
void fs_reader_task(void *params) {
    const taskParam *task_param = static_cast<taskParam *>(params);

    const String str_from_ble = String(task_param->filename);

    // String format will be fs-filename so skip the -
    const String fs = str_from_ble.substring(0, 2);
    const String filename = str_from_ble.substring(3);

    FS *filesystem;
    if (fs == "sd") {
        if (sdcardMounted) {
            filesystem = &SD;
        } else {
            task_param->characteristic->setValue(-1);
            goto task_delete;
        }
    } else if (fs == "lf") {
        filesystem = &LittleFS;
    } else {
        task_param->characteristic->setValue(-4);
        goto task_delete;
    }

    if ((filesystem->exists(filename))) {
        File editFile = filesystem->open(filename, FILE_READ);
        if (editFile) {
            const String fileContent = editFile.readString();
            task_param->characteristic->setValue(fileContent);
            editFile.close();
        } else {
            task_param->characteristic->setValue(-3);
        }
    } else {
        task_param->characteristic->setValue(-2);
    }

task_delete:
    free(task_param->filename);
    free(params);
    vTaskDelete(nullptr);
}

class FSReadCallback: public NimBLECharacteristicCallbacks {
    public:
    void onWrite(NimBLECharacteristic *pCharacteristic) override {
        taskParam *param = static_cast<taskParam *>(malloc(sizeof(taskParam)));

        const String res = pCharacteristic->getValue();
        param->filename = static_cast<char *>(malloc(sizeof(char *) * (res.length() + 1))); // + 1 for null terminator
        strcpy(param->filename, res.c_str());
        param->characteristic = pCharacteristic;
        xTaskCreate(fs_reader_task, "fs_reader_task", 8192, param, tskIDLE_PRIORITY, nullptr);
    }
};

void FileManagerService::setup(NimBLEServer *pServer) {
    sdcardMounted = setupSdCard();
    fs_service = pServer->createService(NimBLEUUID("3d643ce9-e564-49e9-b749-fd17eb18674a"));

    fs_char = fs_service->createCharacteristic(
        NimBLEUUID("824015b3-c4a4-47b4-983c-186226fb365e"),
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    fs_char->setCallbacks(new FSReadCallback());

    tft.setLogging();
    
    fs_service->start();

    pServer->getAdvertising()->addServiceUUID(fs_service->getUUID());
}

void FileManagerService::end() {
    // fs_service->stop();
}
