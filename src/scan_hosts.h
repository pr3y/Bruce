
#include <WiFi.h>
#include <ESP32Ping.h>

void ping_sweep();

bool ping(IPAddress target);

int etharp_get_entry(size_t i, ip4_addr_t **ipaddr, struct netif **netif, struct eth_addr **eth_ret);

void local_net_scan_setup();

void print_arp_table();