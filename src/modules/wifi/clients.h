#include <WiFi.h>

void telnet_setup();

void ssh_setup(String host = "");

void ssh_loop(void *pvParameters);

char* stringTochar(String s);