/**
 * @file tag_o_matic.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read and Write RFID tags
 * @version 0.2
 * @date 2024-08-19
 */

#ifndef __TAG_O_MATIC_H__
#define __TAG_O_MATIC_H__

#include "RFIDInterface.h"
#include <set>

class TagOMatic {
public:
    enum RFID_State {
        READ_MODE,
        SCAN_MODE,
        CLONE_MODE,
        CUSTOM_UID_MODE,
        WRITE_MODE,
        WRITE_NDEF_MODE,
        ERASE_MODE,
        LOAD_MODE,
        SAVE_MODE
    };

    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    TagOMatic();
    TagOMatic(RFID_State initial_state);
    ~TagOMatic();

    /////////////////////////////////////////////////////////////////////////////////////
    // Life Cycle and Setup
    /////////////////////////////////////////////////////////////////////////////////////
    void setup();
    void loop();
    void set_rfid_module();

private:
    RFIDInterface *_rfid;
    RFID_State _initial_state;
    bool _read_uid = false;
    bool _ndef_created = false;
    uint32_t _lastReadTime = 0;
    RFID_State current_state;
    std::set<String> _scanned_set;
    std::vector<String> _scanned_tags;

    /////////////////////////////////////////////////////////////////////////////////////
    // Display functions
    /////////////////////////////////////////////////////////////////////////////////////
    void display_banner();
    void dump_card_details();
    void dump_ndef_details();
    void dump_scan_results();

    /////////////////////////////////////////////////////////////////////////////////////
    // State management
    /////////////////////////////////////////////////////////////////////////////////////
    void select_state();
    void set_state(RFID_State state);

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    void read_card();
    void scan_cards();
    void write_custom_uid();
    void clone_card();
    void erase_card();
    void write_data();
    void write_ndef_data();
    void save_file();
    void save_scan_result();
    void load_file();

    /////////////////////////////////////////////////////////////////////////////////////
    // NDEF
    /////////////////////////////////////////////////////////////////////////////////////
    void create_ndef_message();
    void create_ndef_url();
    void create_ndef_text();
};

#endif
