#ifndef __BJS_INTERPRETER_H__
#define __BJS_INTERPRETER_H__

#include "core/display.h"
#include <duktape.h>

// Credits to https://github.com/justinknight93/Doolittle
// This functionality is dedicated to @justinknight93 for providing such a nice example! Consider yourself a
// part of the team!

class InterpreterJS {
public:
    InterpreterJS(int id, const char *script);
    ~InterpreterJS();

    void start();
    void terminate();
    bool isRunning() const;
    int getId() const;
    duk_context *getContext();

private:
    int id;
    const char *script;
    bool shouldTerminate;
    TaskHandle_t taskHandle = nullptr;
    duk_context *ctx = nullptr;
};

void run_bjs_script();

void interpreterHandler(void *pvParameters);

bool run_bjs_script_headless(char *code);
bool run_bjs_script_headless(FS fs, String filename);

#endif
