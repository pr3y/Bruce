#ifndef __RF_UTILS_H__
#define __RF_UTILS_H__

#include "structs.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>

extern const float subghz_frequency_list[57];
extern const char *subghz_frequency_ranges[];
extern const int range_limits[4][2];

bool initRfModule(String mode = "", float frequency = 0);
void deinitRfModule();
void initCC1101once(SPIClass *SSPI);

void setMHZ(float frequency);
int find_pulse_index(const std::vector<int> &indexed_durations, int duration);
uint64_t crc64_ecma(const std::vector<int> &data);

void addToRecentCodes(struct RfCodes rfcode);
struct RfCodes selectRecentRfMenu();

#endif
