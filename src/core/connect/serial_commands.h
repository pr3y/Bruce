#ifndef __ESP_SERIAL_CMD_H__
#define __ESP_SERIAL_CMD_H__

#include "esp_connection.h"

/**
 * @brief The EspSerialCmd class.
 *
 * This class is responsible for managing the serial commands.
 */
class EspSerialCmd : public EspConnection {
public:
    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    EspSerialCmd();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    /**
     * @brief Sends commands.
     */
    void sendCommands();
    /**
     * @brief Receives commands.
     */
    void receiveCommands();

private:
    String recvCommand;

    /////////////////////////////////////////////////////////////////////////////////////
    // Helpers
    /////////////////////////////////////////////////////////////////////////////////////
    /**
     * @brief Displays the banner.
     */
    void displayBanner();

    /**
     * @brief Displays the received command.
     *
     * @param success Whether the command was successful.
     */
    void displayRecvCommand(bool success);
    /**
     * @brief Displays the receive error.
     */
    void displayRecvError();
    /**
     * @brief Displays the receive footer.
     */
    void displayRecvFooter();

    /**
     * @brief Displays the sent command.
     *
     * @param command The command.
     */
    void displaySentCommand(const char *command);
    /**
     * @brief Displays the sent error.
     */
    void displaySentError();
    /**
     * @brief Displays the sent footer.
     */
    void displaySentFooter();

    /**
     * @brief Creates a command message.
     *
     * @return Message The command message.
     */
    Message createCmdMessage();
};

#endif
