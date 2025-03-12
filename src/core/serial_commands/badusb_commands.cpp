#include "badusb_commands.h"
#include "core/sd_functions.h"
#include "helpers.h"
#include "modules/others/bad_usb.h"

uint32_t badusbFileCallback(cmd *c) {
    // badusb run_from_file HelloWorld.txt

    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.indexOf(".txt") == -1) {
        Serial.println("Invalid filename");
        return false;
    }
    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return false;

    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return false;
    }

    Kb.begin();
    USB.begin();
    key_input(*fs, filepath);

    // TODO: need to reinit serial when finished
    // Kb.end();
    // USB.~ESPUSB(); // Explicit call to destructor
    // Serial.begin(115200);

    return true;
}

uint32_t badusbBufferCallback(cmd *c) {
    if (!(_setupPsramFs())) return false;

    char *txt = _readFileFromSerial();
    String tmpfilepath = "/tmpramfile"; // TODO: Change to use char *txt directly
    File f = PSRamFS.open(tmpfilepath, FILE_WRITE);
    if (!f) return false;

    f.write((const uint8_t *)txt, strlen(txt));
    f.close();
    free(txt);

    Kb.begin();
    USB.begin();
    key_input(PSRamFS, tmpfilepath);

    PSRamFS.remove(tmpfilepath);
    return true;
}

void createBadUsbCommands(SimpleCLI *cli) {
    Command badusbCmd = cli->addCompositeCmd("bu,badusb");

    Command fileCmd = badusbCmd.addCommand("run_from_file", badusbFileCallback);
    fileCmd.addPosArg("filepath");

    Command bufferCmd = badusbCmd.addCommand("run_from_buffer", badusbBufferCallback);
}
