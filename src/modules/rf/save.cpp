#include "save.h"

bool rf_raw_save(RawRecording recorded) {
    FS *fs;

    if (!getFsStorage(fs)) {
        displayError("No space left on device", true);
        return false;
    }

    String filename = "";
    String rawDataLine = "RAW_Data: ";

    String subfile_out = "Filetype: Bruce SubGhz File\nVersion 1\n";
    subfile_out += "Frequency: " + String(int(recorded.frequency * 1000000)) + "\n";
    subfile_out += "Preset: 0\n";
    subfile_out += "Protocol: RAW\n";

    subfile_out += rawDataLine;

    filename = "raw_";
    int i = 0;
    File file;

    if (!(*fs).exists("/BruceRF")) {
        if (!(*fs).mkdir("/BruceRF")) {
            displayError("Error creating directory", true);
            return false;
        }
    }
    while ((*fs).exists("/BruceRF/" + filename + String(i) + ".sub")) { i++; }

    file = (*fs).open("/BruceRF/" + filename + String(i) + ".sub", FILE_WRITE);
    if (!file) {
        displayError("Error creating file", true);
        return false;
    }

    file.println(subfile_out);

    for (size_t i = 0; i < recorded.codes.size(); ++i) {
        for (size_t j = 0; j < recorded.codeLengths[i]; ++j) {
            if (recorded.codes[i][j].duration0 > 0) {
                if (recorded.codes[i][j].level0 != 1) rawDataLine += "-";
                file.print(String(recorded.codes[i][j].duration0));
                file.print(" ");
            }
            if (recorded.codes[i][j].duration1 > 0) {
                if (recorded.codes[i][j].level1 != 1) rawDataLine += "-";
                file.print(String(recorded.codes[i][j].duration1));
                file.print(" ");
            }
        }
        if (i < recorded.codes.size() - 1) {
            file.print(String(recorded.gaps[i] * -1000));
            file.print(" ");
        }
    }

    file.close();
    displaySuccess("/BruceRF/" + filename + String(i) + ".sub");

    return true;
}

