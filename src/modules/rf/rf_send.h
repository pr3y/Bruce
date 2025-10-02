#ifndef __RF_SEND_H__
#define __RF_SEND_H__

#include "structs.h"

void sendCustomRF();
bool txSubFile(FS *fs, String filepath, bool hideDefaultUI = false);

void sendRfCommand(struct RfCodes rfcode, bool hideDefaultUI = false);
void RCSwitch_send(uint64_t data, unsigned int bits, int pulse = 0, int protocol = 1, int repeat = 10);

void RCSwitch_RAW_Bit_send(RfCodes data);
void RCSwitch_RAW_send(int *ptrtransmittimings);

#endif
