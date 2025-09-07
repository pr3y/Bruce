#ifndef __BJS_INTERPRETER_H__
#define __BJS_INTERPRETER_H__

#include "core/display.h"
#include "task_manager.h"
#include <duktape.h>

// Credits to https://github.com/justinknight93/Doolittle
// This functionality is dedicated to @justinknight93 for providing such a nice example! Consider yourself a
// part of the team!

class InterpreterJS : Task {
public:
    InterpreterJS(char *script, const char *scriptName = NULL, const char *scriptDirpath = NULL);
    ~InterpreterJS();

    const char *getName() { return scriptName.c_str(); };
    const char *getScriptDirpath() { return scriptDirpath.c_str(); };
    const char *getScript() { return script; };

    void toForeground();
    void toBackground();

    void terminate(bool waitForTermination = false);

    bool shouldTerminate = false;
    bool isForeground = true;

    int _taskId;
    bool _isExecuting = false;

private:
    char *script;
    String scriptName;
    String scriptDirpath;

    TaskHandle_t taskHandle = nullptr;
    duk_context *ctx = nullptr;
};

void run_bjs_script();

void interpreterHandler(void *pvParameters);

bool run_bjs_script_headless(char *code);
bool run_bjs_script_headless(FS &fs, String filename);

#endif
