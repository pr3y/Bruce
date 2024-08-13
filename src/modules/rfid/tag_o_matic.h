/**
 * @file tag_o_matic.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read and Write RFID tags
 * @version 0.1
 * @date 2024-07-17
 */

#include "mfrc522_i2c.h"
#include <Wire.h>
#include "core/globals.h"


struct PrintableUID{
	String uid;
	String bcc;
	String sak;
	String atqa;
	String picc_type;
};

struct NdefMessage{
	byte begin = 0x03;
	byte messageSize;
	byte header = 0xD1;
	byte tnf = 0x01;
	byte payloadSize;
	byte payloadType;
	byte payload[140];
	byte end = 0xFE;
};

class TagOMatic {
public:
	enum RFID_State {
		READ_MODE,
		CLONE_MODE,
		WRITE_MODE,
		WRITE_NDEF_MODE,
		ERASE_MODE,
		LOAD_MODE,
		SAVE_MODE
  };
	enum NDEF_Payload_Type {
		NDEF_TEXT = 0x54,
		NDEF_URI = 0x55
  };

  MFRC522 mfrc522;


	/////////////////////////////////////////////////////////////////////////////////////
	// Constructor
	/////////////////////////////////////////////////////////////////////////////////////
	TagOMatic();
	TagOMatic(RFID_State initial_state);

	/////////////////////////////////////////////////////////////////////////////////////
	// Arduino Life Cycle and Setup
	/////////////////////////////////////////////////////////////////////////////////////
	void setup();
	void loop();
	bool setup_mfrc522();

private:
	RFID_State _initial_state;
	bool _read_uid = false;
	bool _ndef_created = false;
  RFID_State current_state;
	MFRC522::Uid uid;
	PrintableUID printableUID;
	NdefMessage ndefMessage;
	String strAllPages = "";
	int totalPages = 0;
	int dataPages = 0;
	bool pageReadSuccess = false;

	/////////////////////////////////////////////////////////////////////////////////////
	// Display functions
	/////////////////////////////////////////////////////////////////////////////////////
  void cls();
  void display_banner();
	void dump_card_details();
	void dump_ndef_details();

	/////////////////////////////////////////////////////////////////////////////////////
	// State management
	/////////////////////////////////////////////////////////////////////////////////////
	void select_state();
  void set_state(RFID_State state);

	/////////////////////////////////////////////////////////////////////////////////////
	// Operations
	/////////////////////////////////////////////////////////////////////////////////////
	void read_card();
	void clone_card();
	void erase_card();
	void write_data();
	void write_ndef_data();
	void save_file();
	void load_file();

	/////////////////////////////////////////////////////////////////////////////////////
	// NDEF
	/////////////////////////////////////////////////////////////////////////////////////
  void create_ndef_message();
  void create_ndef_url();
  void create_ndef_text();
  bool write_ndef_blocks();

	/////////////////////////////////////////////////////////////////////////////////////
	// File handlers
	/////////////////////////////////////////////////////////////////////////////////////
  bool write_file(String filename);
  bool load_from_file();

	/////////////////////////////////////////////////////////////////////////////////////
	// Converters
	/////////////////////////////////////////////////////////////////////////////////////
  void format_data();
  void parse_data();

	/////////////////////////////////////////////////////////////////////////////////////
	// PICC Helpers
	/////////////////////////////////////////////////////////////////////////////////////
	bool PICC_IsNewCardPresent();

	String get_tag_type();
	bool read_data_blocks();
	bool read_mifare_classic_data_blocks(byte piccType, MFRC522::MIFARE_Key *key);
	bool read_mifare_classic_data_sector(MFRC522::MIFARE_Key *key, byte sector);
	bool read_mifare_ultralight_data_blocks();

	bool write_data_blocks();
	bool write_mifare_classic_data_block(int block, String data);
	bool write_mifare_ultralight_data_block(int block, String data);

	bool erase_data_blocks();
};
