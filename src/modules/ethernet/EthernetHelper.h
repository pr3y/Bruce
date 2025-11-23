#if !defined(LITE_VERSION)

#ifndef ETHERNET_HELPER_H
#define ETHERNET_HELPER_H
#include "driver/spi_master.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include <Arduino.h>
#include <stdint.h>

class EthernetHelper {
private:
    uint8_t mac[6];
    void generate_mac();
    esp_netif_t *eth_netif_spi = NULL;
    esp_eth_handle_t eth_handle_spi = {NULL};
    spi_device_handle_t spi_handle = NULL;
    esp_eth_netif_glue_handle_t eth_glue = NULL;

public:
    EthernetHelper(/* args */);
    ~EthernetHelper();
    bool setup();
    bool is_connected();
    void stop();
};

#endif // ETHERNET_H
#endif
