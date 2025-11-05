#include "helpers.h"
#ifndef LITE_VERSION
#include <globals.h>

bool _setupPsramFs() {
    // https://github.com/tobozo/ESP32-PsRamFS/blob/main/examples/PSRamFS_Test/PSRamFS_Test.ino
    static bool psRamFSMounted = false;
    if (psRamFSMounted) return true; // avoid reinit

#ifdef BOARD_HAS_PSRAM
    PSRamFS.setPartitionSize(ESP.getFreePsram() / 2); // use half of psram
#else
    PSRamFS.setPartitionSize(SAFE_STACK_BUFFER_SIZE);
#endif

    if (!PSRamFS.begin()) {
        serialDevice->println("PSRamFS Mount Failed");
        psRamFSMounted = false;
        return false;
    }
    // else
    psRamFSMounted = true;
    return true;
}

char *_readFileFromSerial(size_t fileSizeChar) {
    char *buf;
    size_t bufSize = 0;
    if (psramFound()) buf = (char *)ps_malloc((fileSizeChar) * sizeof(char));
    else buf = (char *)malloc((fileSizeChar) * sizeof(char));
    if (buf == NULL) {
        serialDevice->printf("Could not allocate %d\n", fileSizeChar);
        return NULL;
    }
    buf[0] = '\0';

    String currLine = "";
    serialDevice->println("Reading input data from serial buffer until EOF");
    serialDevice->flush();
    while (true) {
        if (!serialDevice->available()) {
            delay(10);
            continue;
        }
        currLine = serialDevice->readStringUntil('\n');
        if (currLine.startsWith("EOF")) break;
        size_t lineLength = currLine.length();
        if ((bufSize + lineLength + 1) > fileSizeChar) break;

        memcpy(buf + bufSize, currLine.c_str(), lineLength);
        bufSize += lineLength;
        buf[bufSize++] = '\n';
    }
    buf[bufSize] = '\0';
    return buf;
}
#endif
