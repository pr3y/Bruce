#ifndef __SERIAL_CMDS_H__
#define __SERIAL_CMDS_H__

#include <Arduino.h>

void startSerialCommandsHandlerTask();

bool parseSerialCommand(const String &command);
#endif
