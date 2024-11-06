#include "eeprom.h"
#include "core/globals.h"
#include "core/settings.h"


/*********************************************************************
**  Function: load_eeprom
**  Load EEPROM data
*********************************************************************/
void load_eeprom() {
    EEPROM.begin(EEPROMSIZE); // open eeprom

    bruceConfig.rotation = EEPROM.read(EEPROM_ROT);
    bruceConfig.dimmerSet = EEPROM.read(EEPROM_DIMMER);
    bruceConfig.bright = EEPROM.read(EEPROM_BRIGHT);
    bruceConfig.irTx = EEPROM.read(EEPROM_IR_TX);
    bruceConfig.irRx = EEPROM.read(EEPROM_IR_RX);
    bruceConfig.rfTx = EEPROM.read(EEPROM_RF_TX);
    bruceConfig.rfRx = EEPROM.read(EEPROM_RF_RX);
    bruceConfig.tmz = EEPROM.read(EEPROM_TMZ);
    bruceConfig.priColor = EEPROM.read(EEPROM_FGCOLOR0) << 8 | EEPROM.read(EEPROM_FGCOLOR1);
    bruceConfig.rfModule = EEPROM.read(EEPROM_RF_MODULE);
    bruceConfig.rfidModule = EEPROM.read(EEPROM_RFID_MODULE);

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
    \n- rfModule  =%03d, \
    \n- rfidModule=%03d, \
    \n*-*-*-*-*-*-*-*-*-*-*",
    bruceConfig.rotation,
    bruceConfig.dimmerSet,
    bruceConfig.bright,
    bruceConfig.irTx,
    bruceConfig.irRx,
    bruceConfig.rfTx,
    bruceConfig.rfRx,
    bruceConfig.tmz,
    bruceConfig.priColor,
    bruceConfig.rfModule,
    bruceConfig.rfidModule
    );

    if (
        bruceConfig.rotation > 3
        || bruceConfig.dimmerSet > 60
        || bruceConfig.bright > 100
        || bruceConfig.irTx > 100
        || bruceConfig.irRx > 100
        || bruceConfig.rfRx > 100
        || bruceConfig.rfTx > 100
        || bruceConfig.tmz > 24
    ) {

        bruceConfig.rotation = ROTATION;
        bruceConfig.dimmerSet = 10;
        bruceConfig.bright = 100;
        bruceConfig.irTx = LED;
        bruceConfig.irRx = GROVE_SCL;
        bruceConfig.rfTx = GROVE_SDA;
        bruceConfig.rfRx = GROVE_SCL;
        bruceConfig.tmz = 0;
        bruceConfig.rfModule = M5_RF_MODULE;
        bruceConfig.rfidModule = M5_RFID2_MODULE;

        EEPROM.write(EEPROM_ROT, bruceConfig.rotation);
        EEPROM.write(EEPROM_DIMMER, bruceConfig.dimmerSet);
        EEPROM.write(EEPROM_BRIGHT, bruceConfig.bright);
        EEPROM.write(EEPROM_IR_TX, bruceConfig.irTx);
        EEPROM.write(EEPROM_IR_RX, bruceConfig.irRx);
        EEPROM.write(EEPROM_RF_TX, bruceConfig.rfTx);
        EEPROM.write(EEPROM_RF_RX, bruceConfig.rfRx);
        EEPROM.write(EEPROM_TMZ, bruceConfig.tmz);
        EEPROM.write(EEPROM_FGCOLOR0, int((bruceConfig.priColor >> 8) & 0x00FF));
        EEPROM.write(EEPROM_FGCOLOR1, int(bruceConfig.priColor & 0x00FF));
        EEPROM.write(EEPROM_RF_MODULE, bruceConfig.rfModule);
        EEPROM.write(EEPROM_RFID_MODULE, bruceConfig.rfidModule);
        EEPROM.writeString(20,"");

        EEPROM.commit();      // Store data to EEPROM
        log_w("One of the eeprom values is invalid");
    }

    EEPROM.end();

    setBrightness(bruceConfig.bright, false);
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

    if(EEPROM.read(EEPROM_ROT) != bruceConfig.rotation) { EEPROM.write(EEPROM_ROT, bruceConfig.rotation); count++; }
    if(EEPROM.read(EEPROM_DIMMER) != bruceConfig.dimmerSet) { EEPROM.write(EEPROM_DIMMER, bruceConfig.dimmerSet); count++; }
    if(EEPROM.read(EEPROM_BRIGHT) != bruceConfig.bright) { EEPROM.write(EEPROM_BRIGHT, bruceConfig.bright);  count++; }

    if(EEPROM.read(EEPROM_IR_TX) != bruceConfig.irTx) { EEPROM.write(EEPROM_IR_TX, bruceConfig.irTx); count++; }
    if(EEPROM.read(EEPROM_IR_RX) != bruceConfig.irRx) { EEPROM.write(EEPROM_IR_RX, bruceConfig.irRx); count++; }
    if(EEPROM.read(EEPROM_RF_TX) != bruceConfig.rfTx) { EEPROM.write(EEPROM_RF_TX, bruceConfig.rfTx); count++; }
    if(EEPROM.read(EEPROM_RF_RX) != bruceConfig.rfRx) { EEPROM.write(EEPROM_RF_RX, bruceConfig.rfRx); count++; }
    if(EEPROM.read(EEPROM_TMZ) != bruceConfig.tmz) { EEPROM.write(EEPROM_TMZ, bruceConfig.tmz); count++; }
    if(EEPROM.read(EEPROM_FGCOLOR0) !=(int((bruceConfig.priColor >> 8) & 0x00FF))) {EEPROM.write(EEPROM_FGCOLOR0, int((bruceConfig.priColor >> 8) & 0x00FF));  count++; }
    if(EEPROM.read(EEPROM_FGCOLOR1) != int(bruceConfig.priColor & 0x00FF)) { EEPROM.write(EEPROM_FGCOLOR1, int(bruceConfig.priColor & 0x00FF)); count++; }
    if(EEPROM.read(EEPROM_RF_MODULE) != bruceConfig.rfModule) { EEPROM.write(EEPROM_RF_MODULE, bruceConfig.rfModule); count++; }
    if(EEPROM.read(EEPROM_RFID_MODULE) != bruceConfig.rfidModule) { EEPROM.write(EEPROM_RFID_MODULE, bruceConfig.rfidModule); count++; }
    // TODO: add rfFreq

    //If something changed, saves the changes on EEPROM.
    if(count > 0) {
        if(!EEPROM.commit()) log_i("fail to write EEPROM");      // Store data to EEPROM
        else log_i("Wrote new conf to EEPROM");
    }

    EEPROM.end();
}
