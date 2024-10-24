/**
 * @file chameleon.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Emulate tags using a Chameleon device
 * @version 0.1
 * @date 2024-10-09
 */


#ifndef __CHAMELEON_H__
#define __CHAMELEON_H__

#include <chameleonUltra.h>
#include <set>


class Chameleon {
public:
    typedef struct {
        String uid;
        String bcc;
        String sak;
        String atqa;
        String piccType;
    } PrintableUID;

    enum AppMode {
        BATTERY_INFO_MODE,
        FACTORY_RESET_MODE,

        LF_READ_MODE,
        LF_CLONE_MODE,
        LF_EMULATION_MODE,
        LF_SAVE_MODE,
        LF_LOAD_MODE,
        LF_CUSTOM_UID_MODE,

        HF_READ_MODE,
        HF_SCAN_MODE,
        HF_EMULATION_MODE,
        HF_SAVE_MODE,
        HF_LOAD_MODE,
        HF_CLONE_MODE,
        HF_WRITE_MODE,
        HF_CUSTOM_UID_MODE,

        // WRITE_NDEF_MODE,
        // ERASE_MODE,
    };


    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    Chameleon();
    ~Chameleon();

    /////////////////////////////////////////////////////////////////////////////////////
    // Life Cycle
    /////////////////////////////////////////////////////////////////////////////////////
    void setup();
    void loop();
    bool connect();

private:
    ChameleonUltra chmUltra = ChameleonUltra(true);
    ChameleonUltra::LfTag lfTagData;
    ChameleonUltra::HfTag hfTagData;
    AppMode currentMode;
    PrintableUID printableHFUID;
    String printableLFUID;
    String dumpFilename = "";
    String strDump = "";
    bool _lf_read_uid = false;
    bool _hf_read_uid = false;
    bool _battery_set = false;
    bool pageReadSuccess = false;
    String strAllPages = "";
    int totalPages = 0;
    int dataPages = 0;
    std::set<String> _scanned_set;
    std::vector<String> _scanned_tags;


    /////////////////////////////////////////////////////////////////////////////////////
    // State management
    /////////////////////////////////////////////////////////////////////////////////////
    void selectMode();
    void setMode(AppMode mode);

    /////////////////////////////////////////////////////////////////////////////////////
    // Display functions
    /////////////////////////////////////////////////////////////////////////////////////
    void displayBanner();
    void dumpHFCardDetails();
    void dumpHFScanResults();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    void readLFTag();
    void cloneLFTag();
    void customLFUid();
    void emulateLF();
    void saveFileLF();
    void loadFileLF();

    void readHFTag();
    void scanHFTags();
    void cloneHFTag();
    void writeHFData();
    void customHFUid();
    void emulateHF();
    void saveFileHF();
    void loadFileHF();

    void getBatteryInfo();
    void factoryReset();
    // void erase_card();
    // void write_ndef_data();

    /////////////////////////////////////////////////////////////////////////////////////
    // Helpers
    /////////////////////////////////////////////////////////////////////////////////////
    void formatLFUID();
    void parseLFUID();
    bool writeFileLF(String filename);
    bool readFileLF();

    void formatHFData();
    void parseHFData();
    bool writeFileHF(String filename);
    bool readFileHF();
    bool readHFDataBlocks();
    bool readMifareClassicDataBlocks(uint8_t *key);
    bool readMifareUltralightDataBlocks();
    bool writeHFDataBlocks();
    void saveHFScanResult();

    uint8_t selectSlot();
    bool isMifareClassic(byte sak);

};

#endif
