#include <stdio.h>
#include <string.h>
#include <WiFi.h>
#include "core/net_utils.h"

#include "lwip/etharp.h"
// sets number of maximum of pending requests to table size
#define ARP_MAXPENDING ARP_TABLE_SIZE

struct Host {
    Host(ip4_addr_t* ip, eth_addr* eth) : ip(ip->addr), mac(MAC(eth->addr)){}
    IPAddress ip;
    String mac;
};

void local_scan_setup();

void hostInfo(const Host& host);

void afterScanOptions(const Host& ip);