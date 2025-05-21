#ifndef __SCAN_HOSTS_H__
#define __SCAN_HOSTS_H__

#include "core/net_utils.h"
#include <FS.h>
#include <WiFi.h>
#include <stdio.h>
#include <string.h>

#include "lwip/etharp.h"
// sets number of maximum of pending requests to table size
#define ARP_MAXPENDING ARP_TABLE_SIZE

struct Host {
    Host(ip4_addr_t *ip, eth_addr *eth) : ip(ip->addr), mac(MAC(eth->addr)) {}
    IPAddress ip;
    String mac;
};

#endif
