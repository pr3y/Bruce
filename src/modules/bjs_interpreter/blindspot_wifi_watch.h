#pragma once
#include <stdint.h>
#include <string.h>
// Bounded per-transmitter counters, fed only with observed management frames.
struct BlindSpotSender {
    bool used = false;
    uint8_t mac[6] = {}, channel = 0;
    int8_t rssi = 0;
    uint16_t total = 0, broadcast = 0, reason = 0, sequence = 0;
    uint32_t start = 0, last = 0;
};
struct BlindSpotWifiWatch {
    BlindSpotSender senders[32] = {};
    void note(const uint8_t *p, unsigned size, uint8_t ch, int8_t rssi, uint32_t now) {
        if (size < 26 || ((p[0] & 0xfc) != 0xc0 && (p[0] & 0xfc) != 0xa0) || (p[1] & 0x40)) return;
        BlindSpotSender *s = nullptr, *oldest = &senders[0];
        for (auto &v : senders) {
            if (v.used && v.channel == ch && !memcmp(v.mac, p + 10, 6)) {
                s = &v;
                break;
            }
            if (!v.used || (uint32_t)(now - v.last) > (uint32_t)(now - oldest->last)) oldest = &v;
        }
        if (!s) {
            s = oldest;
            *s = BlindSpotSender{};
            s->used = true;
            memcpy(s->mac, p + 10, 6);
            s->channel = ch;
            s->start = now;
        }
        const uint16_t seq = p[22] | (uint16_t)p[23] << 8;
        if (s->total && (p[1] & 8) && s->sequence == seq && now - s->last < 200) return;
        if (now - s->start > 3000) {
            s->start = now;
            s->total = s->broadcast = 0;
        }
        s->last = now;
        s->sequence = seq;
        s->rssi = rssi;
        s->reason = p[24] | (uint16_t)p[25] << 8;
        if (s->total < 65535) ++s->total;
        bool bc = true;
        for (unsigned i = 4; i < 10; ++i)
            if (p[i] != 255) bc = false;
        if (bc && s->broadcast < 65535) ++s->broadcast;
    }
};
