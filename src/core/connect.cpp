/**
 * @file connect.cpp
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Connection between devices
 * @version 0.1
 * @date 2024-10-02
 */


#include "connect.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include "core/mykeyboard.h"
#include "core/display.h"


DeviceConnection::Status espnowSendStatus;
std::vector<DeviceConnection::FileMessage> recvQueue;


DeviceConnection::DeviceConnection() {}
DeviceConnection::~DeviceConnection() { esp_now_deinit(); }


void DeviceConnection::sendFile() {
    drawMainBorderWithTitle("SEND FILE");

    sendStatus = CONNECTING;
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        displayError("Error initializing share");
        returnToMenu=true;
        delay(1000);
        return;
    }

    if (!setupPeer()) {
        displayError("Failed to add peer");
        returnToMenu=true;
        delay(1000);
        return;
    }

    File file = selectFile();
    if (!file) {
        displayError("Error selecting file");
        returnToMenu=true;
        delay(1000);
        return;
    }

    FileMessage message = createFileMessage(file);

    esp_now_register_send_cb(onDataSent);

    esp_err_t response;
    sendStatus = STARTED;
    espnowSendStatus = STARTED;

    drawMainBorderWithTitle("SEND FILE");
    padprintln("");
    padprintln("Sending...");

    delay(100);

    while (file.available()) {
        if (espnowSendStatus == FAILED) sendStatus = FAILED;

        if (checkEscPress()) sendStatus = ABORTED;

        if (sendStatus == ABORTED || sendStatus == FAILED) {
            message.done = true;
            message.dataSize = 0;
            esp_now_send(broadcastAddress, (uint8_t*)&message, sizeof(message));
            displayError("Error sending file");
            break;
        }

        size_t bytesRead = file.readBytes(message.data, PAGE_BYTES);
        message.dataSize = bytesRead;
        message.bytesSent = min(message.bytesSent+bytesRead, message.totalBytes);
        message.done = message.bytesSent == message.totalBytes;

        response = esp_now_send(broadcastAddress, (uint8_t*)&message, sizeof(message));
        if (response != ESP_OK) {
            Serial.printf("Send file response: %s\n", esp_err_to_name(response));
            sendStatus = FAILED;
        }

        progressHandler(file.position(), file.size(), "Sending...");
        delay(100);
    }

    if (message.bytesSent == message.totalBytes) displaySuccess("File sent");

    file.close();
    esp_now_unregister_send_cb();
    returnToMenu=true;
    delay(1000);
}


void DeviceConnection::receiveFile() {
    drawMainBorderWithTitle("RECEIVE FILE");
    padprintln("");
    padprintln("Waiting...");

    recvFileName = "";
    recvQueue = {};
    recvStatus = CONNECTING;
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        displayError("Error initializing share");
        delay(1000);
        return;
    }

    esp_now_register_recv_cb(onDataRecv);

    delay(100);

    while(1) {
        if (checkEscPress()) recvStatus = ABORTED;

        if (recvStatus == ABORTED || recvStatus == FAILED) {
            displayError("Error receiving file");
            break;
        }
        if (recvStatus == SUCCESS) {
            displaySuccess("File received");
            break;
        }

        if (!recvQueue.empty()) {
            FileMessage recvFileMessage = recvQueue.front();
            recvQueue.erase(recvQueue.begin());

            progressHandler(recvFileMessage.bytesSent, recvFileMessage.totalBytes, "Receiving...");

            if (!appendToFile(recvFileMessage)) {
                recvStatus = FAILED;
                Serial.println("Append file fail");
            }
            if (recvFileMessage.done) {
                Serial.println("Recv done");
                recvStatus = (
                    recvFileMessage.bytesSent == recvFileMessage.totalBytes
                    ? SUCCESS
                    : FAILED
                );
            }
        }

        delay(100);
    }

    esp_now_unregister_recv_cb();
    delay(1000);

    if (recvStatus == SUCCESS) {
        drawMainBorderWithTitle("RECEIVE FILE");
        padprintln("");
        padprintln("File received: ");
        padprintln(recvFileName);
        padprintln("\n");
        padprintln("Press any key to leave");
        while(!checkAnyKeyPress()) { delay(80); }
    }
}


bool DeviceConnection::setupPeer() {
    peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);

    Serial.print("Adding peer: ");
    for (byte i = 0; i < 6; i++) {
        if (i>0) Serial.print(":");
        Serial.print(broadcastAddress[i] < 0x10 ? "0" : "");
        Serial.print(broadcastAddress[i], HEX);
    }
    Serial.println();

    esp_err_t add_peer_resp = esp_now_add_peer(&peerInfo);
    // Serial.printf("Add peer response: 0X%x\n", add_peer_resp);

    return add_peer_resp == ESP_OK;
}


File DeviceConnection::selectFile() {
    String filename;
    FS *fs=&LittleFS;
    setupSdCard();
    if(sdcardMounted) {
        options = {
            {"Sd Card",  [&](){ fs=&SD; }},
            {"LittleFs", [&](){ fs=&LittleFS; }},
        };
        delay(200);
        loopOptions(options);
        delay(200);
    }
    filename = loopSD(*fs,true);

    File file = fs->open(filename, FILE_READ);
    return file;
}


DeviceConnection::FileMessage DeviceConnection::createFileMessage(File file) {
    FileMessage message;
    String path = String(file.path());

    message.filename = file.name();
    message.filepath = path.substring(0, path.lastIndexOf("/"));
    message.totalBytes = file.size();
    message.bytesSent = 0;
    message.done = false;

    return message;
}


bool DeviceConnection::appendToFile(DeviceConnection::FileMessage fileMessage) {
    FS *fs;
    if(!getFsStorage(fs)) return false;

    if (recvFileName == "") createFilename(fs, fileMessage);

    File file = (*fs).open(recvFileName, FILE_APPEND);
    if(!file) return false;

    file.write((const uint8_t*)fileMessage.data, fileMessage.dataSize);
    file.close();

    return true;
}


void DeviceConnection::createFilename(FS *fs, DeviceConnection::FileMessage fileMessage) {
    String filename = fileMessage.filename.substring(0,fileMessage.filename.lastIndexOf("."));
    String ext = fileMessage.filename.substring(fileMessage.filename.lastIndexOf("."));

    Serial.println("Creating file");
    Serial.print("Path: ");Serial.println(fileMessage.filepath);
    Serial.print("Name: ");Serial.println(filename);
    Serial.print("Ext: ");Serial.println(ext);

    if (!(*fs).exists(fileMessage.filepath)) (*fs).mkdir(fileMessage.filepath);
    if ((*fs).exists(fileMessage.filepath+"/"+filename+ext)) {
        int i = 1;
        filename += "_";
        while((*fs).exists(fileMessage.filepath+"/" + filename + String(i) + ext)) i++;
        filename += String(i);
    }

    recvFileName = fileMessage.filepath+"/" + filename + ext;
}


void onDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        espnowSendStatus = DeviceConnection::SUCCESS;
        Serial.println("ESPNOW send success");
    } else {
        espnowSendStatus = DeviceConnection::FAILED;
        Serial.println("ESPNOW send fail");
    }
}


void onDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
    DeviceConnection::FileMessage recvFileMessage;
    memcpy(&recvFileMessage, incomingData, sizeof(recvFileMessage));

    Serial.print("Name: ");
    Serial.println(recvFileMessage.filename);
    Serial.print("Path: ");
    Serial.println(recvFileMessage.filepath);
    Serial.print("Bytes sent: ");
    Serial.println(recvFileMessage.bytesSent);
    Serial.print("Total bytes: ");
    Serial.println(recvFileMessage.totalBytes);
    Serial.print("Done: ");
    Serial.println(recvFileMessage.done);
    // Serial.println("Data: ");
    // Serial.write((const uint8_t*)recvFileMessage.data, recvFileMessage.dataSize);
    Serial.println("\n-----");

    recvQueue.push_back(recvFileMessage);
}
