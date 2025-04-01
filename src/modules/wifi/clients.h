#ifndef __CLIENTS_H__
#define __CLIENTS_H__

#include <WiFi.h>

void telnet_setup();

void ssh_setup(String host = "");

void ssh_loop(void *pvParameters);

char *stringTochar(String s);

#endif
