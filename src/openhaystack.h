#include <esp_bt_defs.h>


void openhaystack_setup();

void openhaystack_loop();

void set_payload_from_key(uint8_t *payload, uint8_t *public_key_decoded);

void set_addr_from_key(esp_bd_addr_t addr, uint8_t *public_key_decoded);

