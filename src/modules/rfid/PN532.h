/**
 * @file PN532.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read and Write RFID tags using PN532 module
 * @version 0.1
 * @date 2024-08-19
 */

#include "RFIDInterface.h"
#include "lib_pn532/Adafruit_PN532.h"


class PN532 : public RFIDInterface {
public:
    enum PICC_Type {
        PICC_TYPE_MIFARE_MINI	= 0x09,	// MIFARE Classic protocol, 320 bytes
        PICC_TYPE_MIFARE_1K		= 0x08,	// MIFARE Classic protocol, 1KB
        PICC_TYPE_MIFARE_4K		= 0x18,	// MIFARE Classic protocol, 4KB
        PICC_TYPE_MIFARE_UL		= 0x00,	// MIFARE Ultralight or Ultralight C
    };

    Adafruit_PN532 nfc = Adafruit_PN532();

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
    byte pn532_packetbuffer[64];
    Uid _tag_read_uid;

    /////////////////////////////////////////////////////////////////////////////////////
    // Converters
    /////////////////////////////////////////////////////////////////////////////////////
    void format_data();
    void parse_data();
    void set_uid();

    /////////////////////////////////////////////////////////////////////////////////////
    // PICC Helpers
    /////////////////////////////////////////////////////////////////////////////////////
    bool PICC_IsNewCardPresent();
    String PICC_GetTypeName(byte sak);
    bool readDetectedPassiveTargetID();

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
