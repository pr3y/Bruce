/**
 * @file PN532.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read and Write RFID tags using PN532 module
 * @version 0.1
 * @date 2024-08-19
 */

#include "RFIDInterface.h"
#include <Adafruit_PN532.h>


class PN532 : public RFIDInterface {
public:
    enum PICC_Type {
        PICC_TYPE_MIFARE_MINI	= 0x09,	// MIFARE Classic protocol, 320 bytes
        PICC_TYPE_MIFARE_1K		= 0x08,	// MIFARE Classic protocol, 1KB
        PICC_TYPE_MIFARE_4K		= 0x18,	// MIFARE Classic protocol, 4KB
        PICC_TYPE_MIFARE_UL		= 0x00,	// MIFARE Ultralight or Ultralight C
    };

    // Devices such as T-Embed CC1101 uses an embedded PN532 that needs the IRQ and RST pins to work
    // If using other device that uses, set -DPN532_IRQ=pin_num and -DPN532_RF_REST=pin_num to platformio.ini
    // of this particular device, should not be used in other devices on I2C mode
    #if defined(PN532_IRQ) && defined(PN532_RF_REST)
        Adafruit_PN532 nfc = Adafruit_PN532(PN532_IRQ,PN532_RF_REST);    
    #else
    Adafruit_PN532 nfc = Adafruit_PN532();
    #endif

    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    PN532(bool use_i2c = true);

    /////////////////////////////////////////////////////////////////////////////////////
    // Life Cycle
    /////////////////////////////////////////////////////////////////////////////////////
    bool begin();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    int read();
    int clone();
    int erase();
    int write();
    int write_ndef();
    int load();
    int save(String filename);

private:
    bool _use_i2c;

    /////////////////////////////////////////////////////////////////////////////////////
    // Converters
    /////////////////////////////////////////////////////////////////////////////////////
    void format_data();
    void parse_data();
    void set_uid();

    /////////////////////////////////////////////////////////////////////////////////////
    // PICC Helpers
    /////////////////////////////////////////////////////////////////////////////////////
    String get_tag_type();
    bool read_data_blocks();
    bool read_mifare_classic_data_blocks(uint8_t *key);
    bool read_mifare_classic_data_sector(uint8_t *key, byte sector);
    bool read_mifare_ultralight_data_blocks();

    int write_data_blocks();
    bool write_mifare_classic_data_block(int block, String data);
    bool write_mifare_ultralight_data_block(int block, String data);

    int erase_data_blocks();
    int write_ndef_blocks();
};
