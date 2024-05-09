#include "lwip/opt.h"
#include "esp_eth.h"
#include "lwip/etharp.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "nvs_flash.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/ethip6.h"
#include "lwip/ip.h"
#include "lwip/ip4_addr.h"
#include "lwip/prot/udp.h"
#include "display.h"
#include "globals.h"
#include "arp.h"

// #define ARP_TABLE_SIZE 30  // TODO: this probably is causing inconsistance


int etharp_get_entry(size_t i, ip4_addr_t **ipaddr, struct netif **netif, struct eth_addr **eth_ret);


void print_arp_table() {
  struct netif *existing_netif = netif_default;

  if (existing_netif != NULL) {
    ip4_addr_t *printed_addresses[ARP_TABLE_SIZE];
    memset(printed_addresses, 0, sizeof(printed_addresses));

    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
      ip4_addr_t *ipaddr;
      struct netif *netif_entry;
      struct eth_addr *eth_ret;

      int result = etharp_get_entry(i, &ipaddr, &netif_entry, &eth_ret);
      delay(1000);

      if (result == 0 && ipaddr != NULL) {
        // Check if the IP address has already been printed
        int is_duplicate = 0;
        for (int j = 0; j < ARP_TABLE_SIZE; j++) {
          if (printed_addresses[j] != NULL && ip4_addr_cmp(ipaddr, printed_addresses[j])) {
            is_duplicate = 1;
            break;
          }
        }

        if (!is_duplicate) {
          Serial.print("IP: ");
          Serial.print(ip4addr_ntoa(ipaddr));
          tft.println(ip4addr_ntoa(ipaddr));
          Serial.print("\tMAC: ");

          char mac_str[18];
          snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                   eth_ret->addr[0], eth_ret->addr[1], eth_ret->addr[2],
                   eth_ret->addr[3], eth_ret->addr[4], eth_ret->addr[5]);

          Serial.print(mac_str);
          Serial.println();

          for (int j = 0; j < ARP_TABLE_SIZE; j++) {
            if (printed_addresses[j] == NULL) {
              printed_addresses[j] = ipaddr;
              break;
            }
          }
        }
      }
    }
  } else {
    Serial.println("No network interface available.");
  }
  delay(1000);
  tft.fillScreen(TFT_BLACK);
}

void local_scan_setup() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);

    Serial.printf("starting ARP scan");

    tft.setTextColor(TFT_DARKGREEN, BGCOLOR);
    tft.println(WiFi.localIP().toString().c_str());
    tft.setTextColor(FGCOLOR, BGCOLOR);
    print_arp_table();

}

