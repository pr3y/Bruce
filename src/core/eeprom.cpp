#include "eeprom.h"
#include "core/globals.h"
#include "core/settings.h"


/*********************************************************************
**  Function: load_eeprom
**  Load EEPROM data
*********************************************************************/
void load_eeprom() {
    EEPROM.begin(EEPROMSIZE); // open eeprom

    rotation = EEPROM.read(EEPROM_ROT);
    dimmerSet = EEPROM.read(EEPROM_DIMMER);
    bright = EEPROM.read(EEPROM_BRIGHT);
    IrTx = EEPROM.read(EEPROM_IR_TX);
    IrRx = EEPROM.read(EEPROM_IR_RX);
    RfTx = EEPROM.read(EEPROM_RF_TX);
    RfRx = EEPROM.read(EEPROM_RF_RX);
    tmz = EEPROM.read(EEPROM_TMZ);
    FGCOLOR = EEPROM.read(EEPROM_FGCOLOR0) << 8 | EEPROM.read(EEPROM_FGCOLOR1);
    RfModule = EEPROM.read(EEPROM_RF_MODULE);
    RfidModule = EEPROM.read(EEPROM_RFID_MODULE);

    log_i("\
    \n*-*EEPROM Settings*-* \
    \n- rotation  =%03d, \
    \n- dimmerSet =%03d, \
    \n- Brightness=%03d, \
    \n- IR Tx Pin =%03d, \
    \n- IR Rx Pin =%03d, \
    \n- RF Tx Pin =%03d, \
    \n- RF Rx Pin =%03d, \
    \n- Time Zone =%03d, \
    \n- FGColor   =0x%04X \
    \n- RfModule  =%03d, \
    \n- RfidModule=%03d, \
    \n*-*-*-*-*-*-*-*-*-*-*",
    rotation, dimmerSet, bright,IrTx, IrRx, RfTx, RfRx, tmz, FGCOLOR, RfModule, RfidModule
    );

    if (
        rotation > 3
        || dimmerSet > 60
        || bright > 100
        || IrTx > 100
        || IrRx > 100
        || RfRx > 100
        || RfTx > 100
        || tmz > 24
    ) {
        rotation = ROTATION;
        dimmerSet = 10;
        bright = 100;
        IrTx = LED;
        IrRx = RXLED;
        RfTx = GROVE_SDA;
        RfRx = GROVE_SCL;
        FGCOLOR = DEFAULTFGCOLOR;
        tmz = 0;
        RfModule = M5_RF_MODULE;
        RfidModule = M5_RFID2_MODULE;

        EEPROM.write(EEPROM_ROT, rotation);
        EEPROM.write(EEPROM_DIMMER, dimmerSet);
        EEPROM.write(EEPROM_BRIGHT, bright);
        EEPROM.write(EEPROM_IR_TX, IrTx);
        EEPROM.write(EEPROM_IR_RX, IrRx);
        EEPROM.write(EEPROM_RF_TX, RfTx);
        EEPROM.write(EEPROM_RF_RX, RfRx);
        EEPROM.write(EEPROM_TMZ, tmz);
        EEPROM.write(EEPROM_FGCOLOR0, int((FGCOLOR >> 8) & 0x00FF));
        EEPROM.write(EEPROM_FGCOLOR1, int(FGCOLOR & 0x00FF));
        EEPROM.write(EEPROM_RF_MODULE, RfModule);
        EEPROM.write(EEPROM_RFID_MODULE, RfidModule);
        EEPROM.writeString(20,"");

        EEPROM.commit();      // Store data to EEPROM
        log_w("One of the eeprom values is invalid");
    }

    EEPROM.end();

    setBrightness(bright, false);
}


/*********************************************************************
**  Function: write_eeprom
**  write EEPROM data
*********************************************************************/
void write_eeprom(int address, uint8_t val) {
    Serial.printf("Writing EEPROM address %d value %d\n", address, val);
    EEPROM.begin(EEPROMSIZE); // open eeprom
    EEPROM.write(address, val); //set the byte
    EEPROM.commit(); // Store data to EEPROM
    EEPROM.end(); // Free EEPROM memory
    Serial.println("Write EEPROM success");
}


/*********************************************************************
**  Function: write_eeprom_string
**  write EEPROM string data
*********************************************************************/
void write_eeprom_string(int address, String val) {
    Serial.printf("Writing EEPROM address %d value %d\n", address, val);
    EEPROM.begin(EEPROMSIZE); // open eeprom
    EEPROM.writeString(address, val); //set the byte
    EEPROM.commit(); // Store data to EEPROM
    EEPROM.end(); // Free EEPROM memory
    Serial.println("Write EEPROM success");
}


/*********************************************************************
**  Function: read_eeprom
**  read EEPROM data
*********************************************************************/
uint8_t read_eeprom(int address) {
    Serial.printf("Reading EEPROM address %d\n", address);
    EEPROM.begin(EEPROMSIZE); // open eeprom
    uint8_t value = EEPROM.read(address); //set the byte
    EEPROM.end(); // Free EEPROM memory
    Serial.printf("EEPROM value = %d\n", value);
    return value;
}


/*********************************************************************
**  Function: read_eeprom_string
**  read EEPROM string data
*********************************************************************/
String read_eeprom_string(int address) {
    Serial.printf("Reading EEPROM address %d\n", address);
    EEPROM.begin(EEPROMSIZE); // open eeprom
    String value = EEPROM.readString(address); //set the byte
    EEPROM.end(); // Free EEPROM memory
    Serial.println("EEPROM value = "+value);
    return value;
}


void sync_eeprom_values(void) {
    int count = 0;

    EEPROM.begin(EEPROMSIZE); // open eeprom

    if(EEPROM.read(EEPROM_ROT) != rotation) { EEPROM.write(EEPROM_ROT, rotation); count++; }
    if(EEPROM.read(EEPROM_DIMMER) != dimmerSet) { EEPROM.write(EEPROM_DIMMER, dimmerSet); count++; }
    if(EEPROM.read(EEPROM_BRIGHT) != bright) { EEPROM.write(EEPROM_BRIGHT, bright);  count++; }

    if(EEPROM.read(EEPROM_IR_TX) != IrTx) { EEPROM.write(EEPROM_IR_TX, IrTx); count++; }
    if(EEPROM.read(EEPROM_IR_RX) != IrRx) { EEPROM.write(EEPROM_IR_RX, IrRx); count++; }
    if(EEPROM.read(EEPROM_RF_TX) != RfTx) { EEPROM.write(EEPROM_RF_TX, RfTx); count++; }
    if(EEPROM.read(EEPROM_RF_RX) != RfRx) { EEPROM.write(EEPROM_RF_RX, RfRx); count++; }
    if(EEPROM.read(EEPROM_TMZ) != tmz) { EEPROM.write(EEPROM_TMZ, tmz); count++; }
    if(EEPROM.read(EEPROM_FGCOLOR0) !=(int((FGCOLOR >> 8) & 0x00FF))) {EEPROM.write(EEPROM_FGCOLOR0, int((FGCOLOR >> 8) & 0x00FF));  count++; }
    if(EEPROM.read(EEPROM_FGCOLOR1) != int(FGCOLOR & 0x00FF)) { EEPROM.write(EEPROM_FGCOLOR1, int(FGCOLOR & 0x00FF)); count++; }
    if(EEPROM.read(EEPROM_RF_MODULE) != RfModule) { EEPROM.write(EEPROM_RF_MODULE, RfModule); count++; }
    if(EEPROM.read(EEPROM_RFID_MODULE) != RfidModule) { EEPROM.write(EEPROM_RFID_MODULE, RfidModule); count++; }
    // TODO: add RfFreq

    //If something changed, saves the changes on EEPROM.
    if(count > 0) {
        if(!EEPROM.commit()) log_i("fail to write EEPROM");      // Store data to EEPROM
        else log_i("Wrote new conf to EEPROM");
    }

    EEPROM.end();
}
