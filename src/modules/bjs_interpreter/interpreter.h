#include "stdio.h"
#include <SPI.h>
#include <SD.h>
#include <duktape.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <string.h>
#include <chrono>

// Credits to https://github.com/justinknight93/Doolittle
// This functionality is dedicated to @justinknight93 for providing such a nice example! Consider yourself a part of the team!


void run_bjs_script();

bool interpreter();

bool run_bjs_script_headless(const char *code);
bool run_bjs_script_headless(FS fs, String filename);

