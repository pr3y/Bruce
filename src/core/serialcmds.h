#ifndef __SERIAL_CMDS_H__
#define __SERIAL_CMDS_H__

#include <Arduino.h>

void handleSerialCommands();

bool processSerialCommand(String cmd_str);

void startSerialCommandsHandlerTask();

#endif