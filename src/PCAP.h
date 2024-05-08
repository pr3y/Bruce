/*
  ===========================================
       Copyright (c) 2017 Stefan Kremser
              github.com/spacehuhn
  ===========================================
*/

#ifndef PCAP_h
#define PCAP_h

#include <Arduino.h>
#include "SPI.h"
#include "mykeyboard.h"
#include "sniffer.h"
#if defined(ESP32)
	#include "FS.h"
	//#include "SD.h"
#else
	#include <SPI.h>
	#include <SdFat.h>
#endif


//String filename;
File file;

/* converts a 32 bit integer into 4 bytes */
void escape32(uint32_t n, uint8_t* buf){
  buf[0] = n;
  buf[1] = n >>  8;
  buf[2] = n >> 16;
  buf[3] = n >> 24;
}

/* converts a 16 bit integer into 2 bytes */
void escape16(uint16_t n, uint8_t* buf){
  buf[0] = n;
  buf[1] = n >>  8;
}

/* writes a 32 bit integer onto the SD card */
void filewrite_32(uint32_t n){
  uint8_t _buf[4];
  escape32(n, _buf);
  file.write(_buf, 4);
}

/* writes a 16 bit integer onto the SD card */
void filewrite_16(uint16_t n){
  uint8_t _buf[2];
  escape16(n, _buf);
  file.write(_buf, 2);
}

/* writes a 32 bit integer to Serial */
void serialwrite_32(uint32_t n){
  uint8_t _buf[4];
  escape32(n, _buf);
  Serial.write(_buf, 4);
}

/* writes a 16 bit integer to Serial */
void serialwrite_16(uint16_t n){
  uint8_t _buf[2];
  escape16(n, _buf);
  Serial.write(_buf, 2);
}


/* write packet to file */
void newPacketSD(uint32_t ts_sec, uint32_t ts_usec, uint32_t len, uint8_t* buf){
  if(file){
    
    uint32_t orig_len = len;
    uint32_t incl_len = len;
    //if(incl_len > snaplen) incl_len = snaplen; /* safty check that the packet isn't too big (I ran into problems here) */
    
    filewrite_32(ts_sec);
    filewrite_32(ts_usec);
    filewrite_32(incl_len);
    filewrite_32(orig_len);
  
    file.write(buf, incl_len);
  }
}


/* write packet to Serial */
void newPacketSerial(uint32_t ts_sec, uint32_t ts_usec, uint32_t len, uint8_t* buf){
  uint32_t orig_len = len;
  uint32_t incl_len = len;
  uint32_t snaplen = 2500;
#if defined(ESP32)
  if(incl_len > snaplen) incl_len = snaplen; /* safty check that the packet isn't too big (I ran into problems with the ESP8266 here) */
#endif
  serialwrite_32(ts_sec);
  serialwrite_32(ts_usec);
  serialwrite_32(incl_len);
  serialwrite_32(orig_len);
  
  Serial.write(buf, incl_len);
}

class PCAP
{

    void startSerial();
#if defined(ESP32)
	bool openFile(fs::FS &fs);
  //  bool removeFile(fs::FS &fs);
#else
	bool openFile(SdFat &SD);
   // bool removeFile(SdFat &SD);
#endif
	
    void flushFile();
    void closeFile();

    void newPacketSerial(uint32_t ts_sec, uint32_t ts_usec, uint32_t len, uint8_t* buf);
	
	  String filename = "/raw.cap";
	
    uint32_t magic_number = 0xa1b2c3d4;
    uint16_t version_major = 2;
    uint16_t version_minor = 4;
    uint32_t thiszone = 0;
    uint32_t sigfigs = 0;
    uint32_t snaplen = 2500;
    uint32_t network = 105;

  private:
    //File file;

    void escape32(uint32_t n, uint8_t* buf);
    void escape16(uint16_t n, uint8_t* buf);
	
    void filewrite_16(uint16_t n);
    void filewrite_32(uint32_t n);
	
    void serialwrite_16(uint16_t n);
    void serialwrite_32(uint32_t n);
};

#endif

