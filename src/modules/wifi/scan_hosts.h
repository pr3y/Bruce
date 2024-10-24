#include <stdio.h>
#include <string.h>
#include <WiFi.h>

#include "lwip/etharp.h"
// sets number of maximum of pending requests to table size
#define ARP_MAXPENDING ARP_TABLE_SIZE

struct Host {
    Host(ip4_addr_t* ip, eth_addr* eth) : ip(ip->addr){
        char macStr[18];  
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                eth->addr[0], eth->addr[1], eth->addr[2],
                eth->addr[3], eth->addr[4], eth->addr[5]);
        mac = macStr;
    }
    IPAddress ip;
    String mac;
};

void local_scan_setup();

void hostInfo(const Host& host);

void afterScanOptions(const Host& ip);