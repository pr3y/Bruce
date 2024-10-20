// html_portal.h
#ifndef HTML_PORTAL_H
#define HTML_PORTAL_H

#include "core/globals.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <SD.h>
#include <SPI.h>

// Другие объявления
void startHtmlPortal();
String getHtmlTemplate();

#endif // HTML_PORTAL_H
