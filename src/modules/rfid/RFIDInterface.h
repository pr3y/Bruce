/**
 * @file RFIDInterface.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Interface for RFID modules
 * @version 0.1
 * @date 2024-08-19
 */


#ifndef __RFID2INTERFACE_H__
#define __RFID2INTERFACE_H__

#include "core/globals.h"


class RFIDInterface {
public:
    typedef struct {
        byte size;
        byte uidByte[10];
        byte sak;
        byte atqaByte[2];
    } Uid;

    typedef struct {
        String uid;
        String bcc;
        String sak;
        String atqa;
        String picc_type;
    } PrintableUID;

    typedef struct{
        byte begin = 0x03;
        byte messageSize;
        byte header = 0xD1;
        byte tnf = 0x01;
        byte payloadSize;
        byte payloadType;
        byte payload[140];
        byte end = 0xFE;
    } NdefMessage;

    enum ReturnCode {
        SUCCESS = 0,
        FAILURE = 1,
        TAG_NOT_PRESENT = 2,
        TAG_NOT_MATCH = 3,
        NOT_IMPLEMENTED = 4,
    };

    enum NDEF_Payload_Type {
        NDEF_TEXT = 0x54,
        NDEF_URI = 0x55
    };


    Uid uid;
    PrintableUID printableUID;
    NdefMessage ndefMessage;
    String strAllPages = "";
    int totalPages = 0;
    int dataPages = 0;
    bool pageReadSuccess = false;


    /////////////////////////////////////////////////////////////////////////////////////
    // Life Cycle
    /////////////////////////////////////////////////////////////////////////////////////
    virtual bool begin() = 0;

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    virtual int read() = 0;
    virtual int clone() = 0;
    virtual int erase() = 0;
    virtual int write() = 0;
    virtual int write_ndef() = 0;
    virtual int load() = 0;
    virtual int save(String filename) = 0;
};

#endif
