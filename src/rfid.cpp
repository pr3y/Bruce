#include "rfid.h"
#include "globals.h"
#include "mykeyboard.h"
#include "display.h"

MFRC522 mfrc522(0x24); // Create MFRC522 instance.

enum state {
  read_mode,
  write_mode
} currentState;

bool readUID;
byte UID[20];
uint8_t UIDLength;

void displayReadMode() {
    tft.setTextSize(1);
    tft.println("  RFID2 I2C MFRC522");
    tft.println("");
    tft.println("Press 'ENTER' to write after reading.");
    tft.println("Ready to read.");
    tft.println("");
}

void displayWriteMode() {

    tft.setTextSize(1);
    tft.println("  RFID2 I2C MFRC522");
    tft.println("");
    tft.println("Press 'ENTER' to read a new card.");
    tft.println("Ready to write.");
    tft.println("");
    displayUID();
}

void cls() {
    tft.setCursor(0, 0);
}

void rfid_setup() {
    tft.fillScreen(BGCOLOR);
    tft.setCursor(0, 0);
    tft.println("RFID");
    Serial.begin(115200);
    Wire.begin(GROVE_SDA,GROVE_SCL);
    mfrc522.PCD_Init();
    currentState = read_mode;
    displayReadMode();
    delay(1000);
    rfid_loop(); // inicia o loop do rfid após o setup
}

void rfid_loop() {
    for(;;) { // loop infinito para segurar a função

        if ((checkSelPress()) && readUID) {
            cls();
            switch (currentState) {
                case read_mode:
                    tft.fillScreen(BGCOLOR);
                    cls();
                    currentState = write_mode;
                    displayWriteMode();
//                    displayRedStripe("Erro 1");
                    delay(300);
                    break;
                case write_mode:
                    tft.fillScreen(BGCOLOR);
                    cls();
                    currentState = read_mode;
                    displayReadMode();
                    readUID = false;

                    delay(300);
                    break;
            }
        }

        if (mfrc522.PICC_IsNewCardPresent()) {
        if (mfrc522.PICC_ReadCardSerial()) {


        switch (currentState) {
            case read_mode:
                tft.fillScreen(BGCOLOR);
                cls();
                displayReadMode();
                readCard();         
                break;  
            case write_mode:
                tft.fillScreen(BGCOLOR);
                cls();
                displayWriteMode();
                writeCard();     
                break;  
        }
        cls();
        mfrc522.PICC_HaltA();
        }
     }
    // Checks para sair do while
                if (checkEscPress()) {
                    returnToMenu=true;
                    break;
                }

    }
    Serial.println();
}

void readCard() {
    MFRC522::PICC_Type piccType = (MFRC522::PICC_Type)mfrc522.PICC_GetType(mfrc522.uid.sak);
    tft.print("");
    tft.print(mfrc522.PICC_GetTypeName(piccType));
    tft.print(" (SAK ");
    tft.print(mfrc522.uid.sak);
    tft.print(")\r\n");
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        tft.println("ITS NOT MIFARE CLASSIC.");
        tft.setCursor(0, 0); // Reset cursor
        delay(1000);
    } else {
        tft.println("");
        readUID = true;
        UIDLength = mfrc522.uid.size;
        for (byte i = 0; i < UIDLength; i++) {
            UID[i] = mfrc522.uid.uidByte[i];
        }
        Serial.println();
        displayUID();
        delay(1000);
    }
}

void displayUID() {
    tft.println("User ID:");
    for (byte i = 0; i < UIDLength; i++) {
        tft.print(UID[i] < 0x10 ? " 0" : " ");
        tft.print(UID[i], HEX);
    }
}

void writeCard() {
    if (mfrc522.MIFARE_SetUid(UID, (byte)UIDLength, true)) {
        tft.println();
        tft.println("Wrote the UID.");
        tft.println();
    } else {
        tft.println();
        tft.println("Writing FAILED.");
    }

    mfrc522.PICC_HaltA();
    delay(1000);
}
