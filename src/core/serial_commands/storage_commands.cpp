#include "storage_commands.h"
#include "core/sd_functions.h"
#include "helpers.h"
#include <globals.h>

uint32_t listCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs) || !(*fs).exists(filepath)) return false;

    File root = fs->open(filepath);
    if (!root || !root.isDirectory()) return false; // not a dir

    File file = root.openNextFile();
    while (file) {
        Serial.print(file.name());
        if (file.isDirectory()) {
            Serial.println("\t<DIR>");
        } else {
            Serial.print("\t");
            Serial.println(file.size());
        }
        // Serial.println(file.path());
        // Serial.println(file.getLastWrite());  // TODO: parse to localtime
        file = root.openNextFile();
    }

    file.close();
    root.close();
    return true;
}

uint32_t readCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return false;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs) || !(*fs).exists(filepath)) return false;

    Serial.println(readSmallFile(*fs, filepath));
    return true;
}

uint32_t md5Callback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return false;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs) || !(*fs).exists(filepath)) return false;

    Serial.println(md5File(*fs, filepath));
    return true;
}

uint32_t crc32Callback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return false;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs) || !(*fs).exists(filepath)) return false;

    Serial.println(crc32File(*fs, filepath));
    return true;
}

uint32_t removeCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return false;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return false;
    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return false;
    }

    if ((*fs).remove(filepath)) {
        Serial.println("File removed");
        return true;
    }

    Serial.println("Error removing file");
    return false;
}

uint32_t writeCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    Argument sizeArg = cmd.getArgument("size");
    String filepath = arg.getValue();
    String sizeStr = arg.getValue();
    filepath.trim();
    int fileSize = sizeStr.toInt();

    if (filepath.length() == 0) return false;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    if (fileSize < SAFE_STACK_BUFFER_SIZE) fileSize = SAFE_STACK_BUFFER_SIZE;

    FS *fs;
    if (!getFsStorage(fs)) return false;

    char *txt = _readFileFromSerial(fileSize + 2);
    if (strlen(txt) == 0) return false;

    File f = fs->open(filepath, FILE_WRITE, true);
    if (!f) return false;

    f.write((const uint8_t *)txt, strlen(txt));
    f.close();
    free(txt);

    Serial.println("File written: " + filepath);
    return true;
}

uint32_t renameCallback(cmd *c) {
    Command cmd(c);

    Argument filepathArg = cmd.getArgument("filepath");
    Argument newNameArg = cmd.getArgument("newName");
    String filepath = filepathArg.getValue();
    String newName = newNameArg.getValue();
    filepath.trim();
    newName.trim();

    if (filepath.length() == 0 || newName.length() == 0) return false;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;
    if (!newName.startsWith("/")) newName = "/" + newName;

    FS *fs;
    if (!getFsStorage(fs)) return false;
    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return false;
    }

    if ((*fs).rename(filepath, newName)) {
        Serial.println("File renamed to '" + newName + "'");
        return true;
    }

    Serial.println("Error renaming file");
    return false;
}

uint32_t copyCallback(cmd *c) {
    Command cmd(c);

    Argument filepathArg = cmd.getArgument("filepath");
    Argument newNameArg = cmd.getArgument("newName");
    String filepath = filepathArg.getValue();
    String newName = newNameArg.getValue();
    filepath.trim();
    newName.trim();

    if (filepath.length() == 0 || newName.length() == 0) return false;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;
    if (!newName.startsWith("/")) newName = "/" + newName;

    FS *fs;
    if (!getFsStorage(fs)) return false;
    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return false;
    }

    bool r;
    fileToCopy = filepath;
    if (pasteFile((*fs), newName)) {
        Serial.println("File copied to '" + newName + "'");
        r = true;
    } else {
        Serial.println("Error copying file");
        r = false;
    }

    fileToCopy = "";
    return r;
}

uint32_t mkdirCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return false;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return false;
    if ((*fs).exists(filepath)) {
        Serial.println("Directory already exists");
        return false;
    }

    if ((*fs).mkdir(filepath)) {
        Serial.println("Directory created");
        return true;
    }

    Serial.println("Error creating directory");
    return false;
}

uint32_t rmdirCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return false;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return false;
    if (!(*fs).exists(filepath)) {
        Serial.println("Directory does not exist");
        return false;
    }

    if ((*fs).rmdir(filepath)) {
        Serial.println("Directory removed");
        return true;
    }

    Serial.println("Error removing directory");
    return false;
}

uint32_t statCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.length() == 0) return false;

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs) || !(*fs).exists(filepath)) return false;

    File file = fs->open(filepath);
    if (!file) return false;

    Serial.println("File: " + filepath);

    Serial.print("Size: ");
    Serial.print(file.size());
    Serial.print("\t\t");
    if (file.isDirectory()) Serial.print("directory");
    else Serial.print("regular file");
    Serial.println("");

    Serial.print("Modify: ");
    Serial.print(file.getLastWrite()); // TODO: parse to localtime
    Serial.println("");

    file.close();
    return true;
}

uint32_t freeStorageCallback(cmd *c) {
    Command cmd(c);
    Argument arg = cmd.getArgument("storage_type");

    if (arg.getValue() == "sd") {
        if (setupSdCard()) {
            uint64_t totalBytes = SD.totalBytes();
            uint64_t usedBytes = SD.usedBytes();
            uint64_t freeBytes = totalBytes - usedBytes;

            Serial.printf("SD Total space: %llu Bytes\n", totalBytes);
            Serial.printf("SD Used space: %llu Bytes\n", usedBytes);
            Serial.printf("SD Free space: %llu Bytes\n", freeBytes);
        } else {
            Serial.println("No SD card installed");
        }
    } else if (arg.getValue() == "littlefs") {

        uint64_t totalBytes = LittleFS.totalBytes();
        uint64_t usedBytes = LittleFS.usedBytes();
        uint64_t freeBytes = totalBytes - usedBytes;

        Serial.printf("LittleFS Total space: %llu Bytes\n", totalBytes);
        Serial.printf("LittleFS Used space: %llu Bytes\n", usedBytes);
        Serial.printf("LittleFS Free space: %llu Bytes\n", freeBytes);
    } else {
        Serial.printf("Invalid arg %s\n", arg.getValue().c_str());
        return false;
    }

    return true;
}

void createListCommand(SimpleCLI *cli) {
    Command cmd = cli->addCommand("ls,dir", listCallback);
    cmd.addPosArg("filepath", "");
}

void createReadCommand(SimpleCLI *cli) {
    Command cmd = cli->addCommand("cat,type", readCallback);
    cmd.addPosArg("filepath");
}

void createMd5Command(SimpleCLI *cli) {
    Command cmd = cli->addCommand("md5", md5Callback);
    cmd.addPosArg("filepath");
}

void createCrc32Command(SimpleCLI *cli) {
    // storage crc32
    Command cmd = cli->addCommand("crc32", crc32Callback);
    cmd.addPosArg("filepath");
}

void createRemoveCommand(SimpleCLI *cli) {
    Command cmd = cli->addCommand("rm,del", removeCallback);
    cmd.addPosArg("filepath");
}

void createMkdirCommand(SimpleCLI *cli) {
    Command cmd = cli->addCommand("md,mkdir", mkdirCallback);
    cmd.addPosArg("filepath");
}

void createRmdirCommand(SimpleCLI *cli) {
    Command cmd = cli->addCommand("rmdir", rmdirCallback);
    cmd.addPosArg("filepath");
}

void createStorageCommand(SimpleCLI *cli) {
    Command cmd = cli->addCompositeCommand("storage");

    Command cmdList = cmd.addCommand("list", listCallback);
    cmdList.addPosArg("filepath", "");

    Command cmdRead = cmd.addCommand("read", readCallback);
    cmdRead.addPosArg("filepath");

    Command cmdRemove = cmd.addCommand("remove", removeCallback);
    cmdRemove.addPosArg("filepath");

    Command cmdWrite = cmd.addCommand("write", writeCallback);
    cmdWrite.addPosArg("filepath");
    cmdWrite.addPosArg("size", "0");

    Command cmdRename = cmd.addCommand("rename", renameCallback);
    cmdRename.addPosArg("filepath");
    cmdRename.addPosArg("newName");

    Command cmdCopy = cmd.addCommand("copy", copyCallback);
    cmdCopy.addPosArg("filepath");
    cmdCopy.addPosArg("newName");

    Command cmdMkdir = cmd.addCommand("mkdir", mkdirCallback);
    cmdMkdir.addPosArg("filepath");

    Command cmdRmdir = cmd.addCommand("rmdir", rmdirCallback);
    cmdRmdir.addPosArg("filepath");

    Command cmdMd5 = cmd.addCommand("md5", md5Callback);
    cmdMd5.addPosArg("filepath");

    Command cmdCrc32 = cmd.addCommand("crc32", crc32Callback);
    cmdCrc32.addPosArg("filepath");

    Command cmdStat = cmd.addCommand("stat", statCallback);
    cmdStat.addPosArg("filepath");

    Command cmdFree = cmd.addCommand("free", freeStorageCallback);
    cmdFree.addPosArg("storage_type");
}

void createStorageCommands(SimpleCLI *cli) {
    createListCommand(cli);
    createReadCommand(cli);
    createRemoveCommand(cli);

    createMkdirCommand(cli);
    createRmdirCommand(cli);

    createMd5Command(cli);
    createCrc32Command(cli);

    createStorageCommand(cli);
}
