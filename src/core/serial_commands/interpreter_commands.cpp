#include "interpreter_commands.h"
#include "core/sd_functions.h"
#include "helpers.h"
#include "modules/bjs_interpreter/interpreter.h"

uint32_t jsFileCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return false;

    if (!(*fs).exists(filepath)) {
        Serial.println("Running inline script");

        char *txt = strdup(filepath.c_str());
        run_bjs_script_headless(txt);
        // *txt is freed by js interpreter
        return true;
    }

    run_bjs_script_headless(*fs, filepath);
    return true;
}

uint32_t jsBufferCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("fileSize");
    String strFileSize = arg.getValue();
    strFileSize.trim();

    int fileSize = strFileSize.toInt();
    char *txt = _readFileFromSerial(fileSize < 2 ? SAFE_STACK_BUFFER_SIZE : (fileSize + 2));

    return run_bjs_script_headless(txt);
    // *txt is freed by js interpreter
}

void createInterpreterCommands(SimpleCLI *cli) {
    Command jsCmd = cli->addCompositeCmd("js,run,interpret/er");

    Command fileCmd = jsCmd.addCommand("run_from_file", jsFileCallback);
    fileCmd.addPosArg("filepath");

    Command bufferCmd = jsCmd.addCommand("run_from_buffer", jsBufferCallback);
    bufferCmd.addPosArg("fileSize");
}
