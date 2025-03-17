#include "save.h"

bool rf_raw_save(RawRecording recorded){
    FS *fs;
    String filename = "";

    if(!getFsStorage(fs)) {
        displayError("No space left on device", true);
        return false;
    }

    String subfile_out = "Filetype: Bruce SubGhz File\nVersion 1\n";
    subfile_out += "Frequency: " + String(int(recorded.frequency * 1000000)) + "\n";
    subfile_out += "Preset: 0\n";
    subfile_out += "Protocol: RAW\n";
    
    String rawDataLine = "RAW_Data: ";
    for (size_t i = 0; i < recorded.codes.size(); ++i) {
        // We should add a RAW_Data line for each code, 
        // but since the .sub file emitter adds too long of a delay between codes, 
        // we fit as many codes as possible in lines of up to 50k characters. 
        // More than 50k characters per line seems to cause file corruption.
        if(rawDataLine.length() > 50000) { 
            subfile_out += rawDataLine;
            subfile_out += "\n";
            rawDataLine = "RAW_Data: ";
        }
        for (size_t j = 0; j < recorded.codeLengths[i]; ++j) {
            if(recorded.codes[i][j].duration0 > 0){
                if(recorded.codes[i][j].level0 != 1) rawDataLine += "-";
                rawDataLine += String(recorded.codes[i][j].duration0) + " ";
            }
            if(recorded.codes[i][j].duration1 > 0){
                if(recorded.codes[i][j].level1 != 1) rawDataLine += "-";
                rawDataLine += String(recorded.codes[i][j].duration1) + " ";
            }
        }
        if (i < recorded.codes.size() - 1) {
            rawDataLine += String(recorded.gaps[i] * -1000) + " ";
        }
    }
    subfile_out += rawDataLine;
    subfile_out += "\n";
    rawDataLine = "";

    filename = "raw_";

    int i = 0;
    File file;

    if (!(*fs).exists("/BruceRF")) (*fs).mkdir("/BruceRF");
    while((*fs).exists("/BruceRF/" + filename + String(i) + ".sub")) i++;

    file = (*fs).open("/BruceRF/" + filename + String(i) + ".sub", FILE_WRITE);

    if (file) {
        file.println(subfile_out);
        displaySuccess("/BruceRF/" + filename + String(i) + ".sub");
    } else {
        displayError("Error saving file", true);
    }

    file.close();

    return true;
}