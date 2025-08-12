/**
 * @file EthernetHelper.cpp
 * @author Andrea Canale (https://github.com/andreock)
 * @brief Ethernet initialization file for W5500 Ethernet SPI card
 * @version 0.1
 * @date 2025-05-20
 */
#if !defined(LITE_VERSION)
#include "EthernetHelper.h"
#include "core/display.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Arduino.h>
#include <SPI.h>
#include <stdio.h>
#include <string.h>
EthernetHelper::EthernetHelper() { setup(); }

EthernetHelper::~EthernetHelper() {}

static const char *TAG = "ETHelper";

#if IDF_TARGET_ESP32S3
#define ETH_SPI_CLOCK_MHZ 36
#else
#define ETH_SPI_CLOCK_MHZ 12
#endif

typedef struct {
    uint8_t spi_cs_gpio;
    uint8_t int_gpio;
    int8_t phy_reset_gpio;
    uint8_t phy_addr;
} spi_eth_module_config_t;

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
        case ETHERNET_EVENT_CONNECTED:
            esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
            Serial.println("Ethernet Link Up");
            Serial.printf(
                "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                mac_addr[0],
                mac_addr[1],
                mac_addr[2],
                mac_addr[3],
                mac_addr[4],
                mac_addr[5]
            );
            break;
        case ETHERNET_EVENT_DISCONNECTED: Serial.println("Ethernet Link Down"); break;
        case ETHERNET_EVENT_START: Serial.println("Ethernet Started"); break;
        case ETHERNET_EVENT_STOP: Serial.println("Ethernet Stopped"); break;
        default: break;
    }
}

bool connected = false;

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;
    connected = true;
    Serial.println("Ethernet Got IP Address");
    Serial.println("~~~~~~~~~~~");
    Serial.printf("ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    Serial.printf("ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    Serial.printf("ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    Serial.println("~~~~~~~~~~~");
}

void EthernetHelper::generate_mac() {
    mac[0] = random(0, 255);
    mac[1] = random(0, 255);
    mac[2] = random(0, 255);
    mac[3] = random(0, 255);
    mac[4] = random(0, 255);
    mac[5] = random(0, 255);
}

void EthernetHelper::setup() {
    generate_mac();

    // Initialize TCP/IP network interface
    ESP_ERROR_CHECK(esp_netif_init());
    // Create default event loop that running in background
    esp_event_loop_create_default();

    // Create instance of esp-netif for SPI Ethernet
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
    esp_netif_config_t cfg_spi = {.base = &esp_netif_config, .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH};

    esp_netif_config.if_key = "ETH_SPI_0";
    esp_netif_config.if_desc = "eth0";
    esp_netif_config.route_prio = 30;
    eth_netif_spi = esp_netif_new(&cfg_spi);

    // Init MAC and PHY configs to default
    eth_mac_config_t mac_config_spi = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config_spi = ETH_PHY_DEFAULT_CONFIG();

    // Install GPIO ISR handler to be able to service SPI Eth modlues interrupts
    gpio_install_isr_service(0);

    // Init SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = bruceConfigPins.W5500_bus.mosi,
        .miso_io_num = bruceConfigPins.W5500_bus.miso,
        .sclk_io_num = bruceConfigPins.W5500_bus.sck,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);

    // Init specific SPI Ethernet module configuration
    spi_eth_module_config_t spi_eth_module_config;
    spi_eth_module_config.spi_cs_gpio = bruceConfigPins.W5500_bus.cs;
    spi_eth_module_config.int_gpio = bruceConfigPins.W5500_bus.io0;
    spi_eth_module_config.phy_reset_gpio = -1;
    spi_eth_module_config.phy_addr = 1;

    // Configure SPI interface and Ethernet driver for specific SPI module
    esp_eth_mac_t *mac_spi;
    esp_eth_phy_t *phy_spi;

    spi_device_interface_config_t devcfg = {
        .command_bits = 16, // Actually it's the address phase in W5500 SPI frame
        .address_bits = 8,  // Actually it's the control phase in W5500 SPI frame
        .mode = 0,
        .clock_speed_hz = ETH_SPI_CLOCK_MHZ * 1000 * 1000,
        .queue_size = 20
    };

    // Set SPI module Chip Select GPIO
    devcfg.spics_io_num = spi_eth_module_config.spi_cs_gpio;

    spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
    // w5500 ethernet driver is based on spi driver
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0))
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(SPI2_HOST, &devcfg);
#else
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(spi_handle);
#endif
    // Set remaining GPIO numbers and configuration used by the SPI module
    w5500_config.int_gpio_num = spi_eth_module_config.int_gpio;
    phy_config_spi.phy_addr = spi_eth_module_config.phy_addr;
    phy_config_spi.reset_gpio_num = spi_eth_module_config.phy_reset_gpio;

    mac_spi = esp_eth_mac_new_w5500(&w5500_config, &mac_config_spi);
    phy_spi = esp_eth_phy_new_w5500(&phy_config_spi);

    esp_eth_config_t eth_config_spi = ETH_DEFAULT_CONFIG(mac_spi, phy_spi);
    ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config_spi, &eth_handle_spi));

    // Configure MAC address
    ESP_ERROR_CHECK(esp_eth_ioctl(eth_handle_spi, ETH_CMD_S_MAC_ADDR, mac));

    eth_glue = esp_eth_new_netif_glue(eth_handle_spi);
    // attach Ethernet driver to TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif_spi, eth_glue));

    // Register user defined event handers
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    ESP_ERROR_CHECK(esp_netif_dhcpc_start(eth_netif_spi));

    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(eth_handle_spi));
}

bool EthernetHelper::is_connected() { return connected; }

void EthernetHelper::stop() {
    connected = false;
    delay(500);
    if (eth_netif_spi != nullptr && eth_handle_spi != nullptr && eth_glue != nullptr) {
        // Unregister event handler
        ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
        ESP_ERROR_CHECK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));

        // Stop DHCP
        ESP_ERROR_CHECK(esp_netif_dhcpc_stop(eth_netif_spi));

        // Stop interface and delete it
        ESP_ERROR_CHECK(esp_eth_stop(eth_handle_spi));
        ESP_ERROR_CHECK(esp_eth_del_netif_glue(eth_glue));
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0))
        // function removed in IDF v5
#else
        ESP_ERROR_CHECK(esp_eth_clear_default_handlers(eth_netif_spi));
#endif
        ESP_ERROR_CHECK(esp_eth_driver_uninstall(eth_handle_spi));
        esp_netif_destroy(eth_netif_spi); // Destroy interface
    }
}
#endif
