#ifndef __RF_UTILS_H__
#define __RF_UTILS_H__

#include "structs.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>

#define RMT_RX_CHANNEL RMT_CHANNEL_6
#define RMT_MAX_PULSES 10000 // Maximum number of pulses to record
#define RMT_CLK_DIV 80       /*!< RMT counter clock divider */
#define RMT_1US_TICKS (80000000 / RMT_CLK_DIV / 1000000)
#define RMT_1MS_TICKS (RMT_1US_TICKS * 1000)
#define SIGNAL_STRENGTH_THRESHOLD 1500 // Adjust this threshold as needed

extern const float subghz_frequency_list[57];
extern const char *subghz_frequency_ranges[];
extern const int range_limits[4][2];
extern bool rmtInstalled;

bool initRfModule(String mode = "", float frequency = 0);
void deinitRfModule();
void initCC1101once(SPIClass *SSPI);

void initRMT();
void deinitRMT();

void setMHZ(float frequency);
int find_pulse_index(const std::vector<int> &indexed_durations, int duration);
uint64_t crc64_ecma(const std::vector<int> &data);

void addToRecentCodes(struct RfCodes rfcode);
struct RfCodes selectRecentRfMenu();

#endif
