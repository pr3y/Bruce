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
uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t dstAddress[6];
std::vector<Option> peerOptions;


DeviceConnection::DeviceConnection() {}
DeviceConnection::~DeviceConnection() {
    esp_now_unregister_send_cb();
    esp_now_unregister_recv_cb();

    esp_now_deinit();
}


void DeviceConnection::sendFile() {
    drawMainBorderWithTitle("SEND FILE");

    sendStatus = CONNECTING;

    if (!espnowBegin()) return;

    sendPing();

    loopOptions(peerOptions);
    if (!setupPeer(dstAddress)) {
        displayError("Failed to add peer");
        delay(1000);
        return;
    }

    File file = selectFile();
    if (!file) {
        displayError("Error selecting file");
        delay(1000);
        return;
    }

    FileMessage message = createFileMessage(file);

    esp_err_t response;
    sendStatus = STARTED;
    espnowSendStatus = STARTED;

    drawMainBorderWithTitle("SEND FILE");
    padprintln("");
    padprintln("Sending...");

    delay(100);

    while (file.available()) {
        if (espnowSendStatus == FAILED) sendStatus = FAILED;

        if (check(EscPress)) sendStatus = ABORTED;

        if (sendStatus == ABORTED || sendStatus == FAILED) {
            message.done = true;
            message.dataSize = 0;
            esp_now_send(dstAddress, (uint8_t*)&message, sizeof(message));
            displayError("Error sending file");
            break;
        }

        size_t bytesRead = file.readBytes(message.data, PAGE_BYTES);
        message.dataSize = bytesRead;
        message.bytesSent = min(message.bytesSent+bytesRead, message.totalBytes);
        message.done = message.bytesSent == message.totalBytes;

        response = esp_now_send(dstAddress, (uint8_t*)&message, sizeof(message));
        if (response != ESP_OK) {
            Serial.printf("Send file response: %s\n", esp_err_to_name(response));
            sendStatus = FAILED;
        }

        progressHandler(file.position(), file.size(), "Sending...");
        delay(100);
    }

    if (message.bytesSent == message.totalBytes) displaySuccess("File sent");

    file.close();
    delay(1000);
}


void DeviceConnection::receiveFile() {
    drawMainBorderWithTitle("RECEIVE FILE");
    padprintln("");
    padprintln("Waiting...");

    recvFileName = "";
    recvQueue = {};
    recvStatus = CONNECTING;

    if (!espnowBegin()) return;

    delay(100);

    while(1) {
        if (check(EscPress)) recvStatus = ABORTED;

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

    delay(1000);

    if (recvStatus == SUCCESS) {
        drawMainBorderWithTitle("RECEIVE FILE");
        padprintln("");
        padprintln("File received: ");
        padprintln(recvFileName);
        padprintln("\n");
        padprintln("Press any key to leave");
        while(!check(AnyKeyPress)) { delay(80); }
    }
}


bool DeviceConnection::espnowBegin() {
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        displayError("Error initializing share");
        delay(1000);
        return false;
    }

    if (!setupPeer(broadcastAddress)) {
        displayError("Failed to add peer");
        delay(1000);
        return false;
    }

    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    return true;
}


File DeviceConnection::selectFile() {
    String filename;
    FS *fs=&LittleFS;
    setupSdCard();
    if(sdcardMounted) {
        options = {
            {"SD Card",  [&](){ fs=&SD; }},
            {"LittleFS", [&](){ fs=&LittleFS; }},
        };
        loopOptions(options);
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


bool setupPeer(const uint8_t* mac) {
    if (esp_now_is_peer_exist(mac)) return true;

    esp_now_peer_info_t peerInfo = {};

    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    return esp_now_add_peer(&peerInfo) == ESP_OK;
}


void appendPeerToList(const uint8_t* mac) {
    peerOptions.push_back(
        {macToString(mac), [=]() { memcpy(dstAddress, mac, 6); }}
    );
}


std::string macToString(const uint8_t* mac) {
    char macStr[18];
    sprintf(macStr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(macStr);
}


void sendPing() {
    peerOptions = {
        {"Broadcast", [=]() { memcpy(dstAddress, broadcastAddress, 6); }},
    };

    DeviceConnection::FileMessage message;
    message.ping = true;

    esp_err_t response = esp_now_send(broadcastAddress, (uint8_t*)&message, sizeof(message));
    if (response != ESP_OK) {
        Serial.printf("Send ping response: %s\n", esp_err_to_name(response));
    }

    delay(500);
}


void sendPong(const uint8_t* mac) {
    DeviceConnection::FileMessage message;
    message.pong = true;

    if (!setupPeer(mac)) return;

    esp_err_t response = esp_now_send(mac, (uint8_t*)&message, sizeof(message));
    if (response != ESP_OK) {
        Serial.printf("Send pong response: %s\n", esp_err_to_name(response));
    }
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

    if (recvFileMessage.ping) return sendPong(mac);
    if (recvFileMessage.pong) return appendPeerToList(mac);

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
