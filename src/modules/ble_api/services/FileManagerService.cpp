#include "FileManagerService.h"
#include "globals.h"
#include <BLEDevice.h>
#include <BLEUtils.h>

FileManagerService::FileManagerService() : BruceBLEService() {}

FileManagerService::~FileManagerService() = default;

typedef struct taskParam {
    char *filename;
    NimBLECharacteristic *characteristic;
    int mtu;
} taskParam;

/*
 * Error code in BLE Characteristic:
 * -1: SD not installed
 * -2: File not found
 * -3: Can't open the file
 * -4: Invalid FS
 * -5: Invalid part
 * -6: Chunk size too small(MTU problem)
 */
void fs_reader_task(void *params) {
    const taskParam *task_param = static_cast<taskParam *>(params);

    const String str_from_ble = String(task_param->filename);

    // Nuovo formato: fs-part-filename
    const String fs = str_from_ble.substring(0, 2);
    const int part = str_from_ble.substring(3, 4).toInt();
    const String filename = str_from_ble.substring(5);
    if (part < 0){
        task_param->characteristic->setValue(-5);
        goto task_delete;
    }

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
            size_t fileSize = editFile.size();

            // Compute chunk info
            const int chunkSize = task_param->mtu - 2 - 5; // 3 bytes for part and totalParts + 5 bytes for ATT header
            Serial.println("File size: " + String(fileSize) + " Chunk size: " + String(chunkSize) + " MTU: " + String(task_param->mtu));
            if (chunkSize <= 0) {
                task_param->characteristic->setValue(-6);
                editFile.close();
                goto task_delete;
            }
            const size_t totalParts = fileSize / chunkSize;
            if (part >= totalParts) {
                task_param->characteristic->setValue(-5);
                editFile.close();
                goto task_delete;
            }

            editFile.seek(part * chunkSize);    // Go to the right position

            uint8_t buffer[2 + chunkSize];
            buffer[0] = part;
            buffer[1] = totalParts;

            // Response will be [part, totalParts, data...]
            const size_t readLen = editFile.read(buffer + 2, chunkSize);  // Read the chunk
            task_param->characteristic->setValue(buffer, readLen + 2);
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
    void onWrite(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc* desc) override {
        taskParam *param = static_cast<taskParam *>(malloc(sizeof(taskParam)));

        const String res = pCharacteristic->getValue();
        param->filename = static_cast<char *>(malloc(sizeof(char *) * (res.length() + 1))); // + 1 for null terminator
        strcpy(param->filename, res.c_str());
        param->characteristic = pCharacteristic;
        param->mtu = pCharacteristic->getService()->getServer()->getPeerMTU(desc->conn_handle);
        xTaskCreate(fs_reader_task, "fs_reader_task", 8192, param, tskIDLE_PRIORITY, nullptr);
    }
};

/*
 * Error code in BLE Characteristic:
 * -1: SD not installed
 * -2: Not a directory
 * -4: Invalid FS
 */
void fs_list_task(void *params) {
    const taskParam *task_param = static_cast<taskParam *>(params);

    Serial.println(ESP.getFreeHeap());
    const String str_from_ble = String(task_param->filename);

    // String format will be fs-filename so skip the -
    const String fs = str_from_ble.substring(0, 2);
    const String folder = str_from_ble.substring(3);

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

    if ((filesystem->exists(folder))) {
        File root = filesystem->open(folder);
        if (!root || !root.isDirectory()) {
            task_param->characteristic->setValue(-2);
            goto task_delete;
        }

        String fileList = "";
        File file = root.openNextFile();
        while (file) {
            fileList += String(file.name());
            if (file.isDirectory()) {
                fileList += " <DIR>\n";
            } else {
                fileList += " " + String(file.size()) + "\n";
            }
            file = root.openNextFile();
        }

        task_param->characteristic->setValue(fileList);
        root.close();
    } else {
        task_param->characteristic->setValue(-2);
    }

task_delete:
    free(task_param->filename);
    free(params);
    vTaskDelete(nullptr);
}

class FSListCallback: public NimBLECharacteristicCallbacks {
public:
    void onWrite(NimBLECharacteristic *pCharacteristic) override {
        taskParam *param = static_cast<taskParam *>(malloc(sizeof(taskParam)));

        const String res = pCharacteristic->getValue();
        param->filename = static_cast<char *>(malloc(sizeof(char *) * (res.length() + 1))); // + 1 for null terminator
        strcpy(param->filename, res.c_str());
        param->characteristic = pCharacteristic;
        xTaskCreate(fs_list_task, "fs_list_task", 8192, param, tskIDLE_PRIORITY, nullptr);
    }
};

void FileManagerService::setup(NimBLEServer *pServer) {
    sdcardMounted = setupSdCard();
    pService = pServer->createService(NimBLEUUID("3d643ce9-e564-49e9-b749-fd17eb18674a"));

    fs_char = pService->createCharacteristic(
        NimBLEUUID("824015b3-c4a4-47b4-983c-186226fb365e"),
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    fs_list_char = pService->createCharacteristic(
        NimBLEUUID("f4c4b8e2-3e2b-4d1c-8f7a-3e5f3c8e2a1b"),
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    fs_list_char->setCallbacks(new FSListCallback());
    fs_char->setCallbacks(new FSReadCallback());

    tft.setLogging();
    
    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void FileManagerService::end() {
    // pService->stop();
}
