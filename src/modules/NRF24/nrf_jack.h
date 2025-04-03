#include "nRF24L01.h"

void scan();
void print_payload_details();
bool transmit();
void start_transmit();
void ms_crypt();
void ms_checksum();
void fingerprint();
void log_checksum();
//uint16_t crc_update(uint16_t crc, uint8_t byte, uint8_t bits);
//uint8_t writeRegister(uint8_t reg, uint8_t value);
//void ms_transmit(uint8_t meta, uint8_t hid);
//void log_transmit(uint8_t meta, uint8_t keys2send[], uint8_t keysLen);

void nrf_jack_setup();
void nrf_jack_loop();
