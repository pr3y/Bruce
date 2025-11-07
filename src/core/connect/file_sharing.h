#ifndef __ESP_FILE_SHARING_H__
#define __ESP_FILE_SHARING_H__

#include "esp_connection.h"

/**
 * @brief The FileSharing class.
 *
 * This class is responsible for managing the file sharing.
 */
class FileSharing : public EspConnection {
public:
    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    FileSharing();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    /**
     * @brief Sends a file.
     */
    void sendFile();
    /**
     * @brief Receives a file.
     */
    void receiveFile();

private:
    String recvFileName;

    /////////////////////////////////////////////////////////////////////////////////////
    // Helpers
    /////////////////////////////////////////////////////////////////////////////////////
    /**
     * @brief Selects a file.
     *
     * @return File The selected file.
     */
    File selectFile();
    /**
     * @brief Appends to a file.
     *
     * @param fileMessage The file message.
     * @return bool True if the append was successful, false otherwise.
     */
    bool appendToFile(Message fileMessage);
    /**
     * @brief Creates a filename.
     *
     * @param fs The file system.
     * @param fileMessage The file message.
     */
    void createFilename(FS *fs, Message fileMessage);
};

#endif
