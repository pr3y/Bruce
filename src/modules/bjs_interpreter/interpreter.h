#ifndef __BJS_INTERPRETER_H__
#define __BJS_INTERPRETER_H__
#ifndef LITE_VERSION
#include "core/display.h"
#include "stdio.h"
#include <SD.h>
#include <SPI.h>
#include <chrono>
#include <string.h>

// Credits to https://github.com/justinknight93/Doolittle
// This functionality is dedicated to @justinknight93 for providing such a nice example! Consider yourself a
// part of the team!

void run_bjs_script();

void interpreterHandler(void *pvParameters);

bool run_bjs_script_headless(char *code);
bool run_bjs_script_headless(FS fs, String filename);

#endif
#endif
