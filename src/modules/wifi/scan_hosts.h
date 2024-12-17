#ifndef __SCAN_HOSTS_H__
#define __SCAN_HOSTS_H__

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

String ipToString(const uint8_t* ip);

bool parseIPString(const String &ipStr, uint8_t *ipArray);

String macToString(const uint8_t* mac);

void stringToMAC(const std::string& macStr, uint8_t MAC[6]);

void getGatewayMAC(uint8_t gatewayMAC[6]);

void sendARPPacket(uint8_t *targetIP, uint8_t *targetMAC, uint8_t *spoofedIP, uint8_t *spoofedMAC, File pcapFile);

void arpSpoofing(const Host& host, bool mitm);

void arpPoisoner();

void stationDeauth(Host host);

#endif