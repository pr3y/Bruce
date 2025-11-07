#ifndef __ESP_CONNECTION_H__
#define __ESP_CONNECTION_H__

#include <esp_now.h>
#include <globals.h>
#include <vector>

#define ESP_FILENAME_SIZE 30
#define ESP_FILEPATH_SIZE 50
#define ESP_DATA_SIZE 150

/**
 * @brief The EspConnection class.
 *
 * This class is responsible for managing the ESP-NOW connection.
 */
class EspConnection {
public:
    /**
     * @brief Enum for the status of the connection.
     */
    enum Status {
        CONNECTING,
        STARTED,
        WAITING,
        FAILED,
        SUCCESS,
        ABORTED,
    };

    // Struct has to be 250 B max
    /**
     * @brief The message struct.
     */
    struct Message {
        char filename[ESP_FILENAME_SIZE];
        char filepath[ESP_FILEPATH_SIZE];
        char data[ESP_DATA_SIZE];
        size_t dataSize;
        size_t totalBytes;
        size_t bytesSent;
        bool isFile;
        bool done;
        bool ping;
        bool pong;

        // Constructor to initialize defaults
        Message()
            : dataSize(0), totalBytes(0), bytesSent(0), isFile(false), done(false), ping(false), pong(false) {
        }
    };

    EspConnection();
    ~EspConnection();

    /**
     * @brief Sets the instance of the EspConnection class.
     *
     * @param conn The instance of the EspConnection class.
     */
    static void setInstance(EspConnection *conn) { instance = conn; }

    /**
     * @brief The static onDataSent function.
     *
     * @param mac_addr The MAC address of the receiver.
     * @param status The status of the sending.
     */
    static void onDataSentStatic(const uint8_t *mac_addr, esp_now_send_status_t status) {
        if (instance) instance->onDataSent(mac_addr, status);
    };
    /**
     * @brief The static onDataRecv function.
     *
     * @param mac The MAC address of the sender.
     * @param incomingData The incoming data.
     * @param len The length of the incoming data.
     */
    static void onDataRecvStatic(const uint8_t *mac, const uint8_t *incomingData, int len) {
        if (instance) instance->onDataRecv(mac, incomingData, len);
    };

protected:
    Status recvStatus;
    Status sendStatus;
    uint8_t dstAddress[6];
    uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    std::vector<Message> recvQueue;

    /**
     * @brief Begins the sending.
     *
     * @return bool True if the sending was successful, false otherwise.
     */
    bool beginSend();
    /**
     * @brief Begins the ESP-NOW connection.
     *
     * @return bool True if the connection was successful, false otherwise.
     */
    bool beginEspnow();

    /**
     * @brief Creates a message.
     *
     * @param text The text of the message.
     * @return Message The message.
     */
    Message createMessage(String text);
    /**
     * @brief Creates a file message.
     *
     * @param file The file.
     * @return Message The message.
     */
    Message createFileMessage(File file);
    /**
     * @brief Creates a ping message.
     *
     * @return Message The message.
     */
    Message createPingMessage();
    /**
     * @brief Creates a pong message.
     *
     * @return Message The message.
     */
    Message createPongMessage();

    /**
     * @brief Sends a ping.
     */
    void sendPing();
    /**
     * @brief Sends a pong.
     *
     * @param mac The MAC address of the receiver.
     */
    void sendPong(const uint8_t *mac);

    /**
     * @brief Sets up a peer.
     *
     * @param mac The MAC address of the peer.
     * @return bool True if the setup was successful, false otherwise.
     */
    bool setupPeer(const uint8_t *mac);
    /**
     * @brief Appends a peer to the list.
     *
     * @param mac The MAC address of the peer.
     */
    void appendPeerToList(const uint8_t *mac);
    /**
     * @brief Sets the destination address.
     *
     * @param address The destination address.
     */
    void setDstAddress(const uint8_t *address) { memcpy(dstAddress, address, 6); }

    /**
     * @brief Converts a MAC address to a string.
     *
     * @param mac The MAC address.
     * @return String The string.
     */
    String macToString(const uint8_t *mac);
    /**
     * @brief Prints a message.
     *
     * @param message The message.
     */
    void printMessage(Message message);

    /**
     * @brief The onDataSent function.
     *
     * @param mac_addr The MAC address of the receiver.
     * @param status The status of the sending.
     */
    void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    /**
     * @brief The onDataRecv function.
     *
     * @param mac The MAC address of the sender.
     * @param incomingData The incoming data.
     * @param len The length of the incoming data.
     */
    void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

private:
    static EspConnection *instance;
};

#endif
