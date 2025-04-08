#include "rf_commands.h"
#include "cJSON.h"
#include "core/sd_functions.h"
#include "helpers.h"
#include "modules/rf/rf_scan.h"
#include "modules/rf/rf_send.h"
#include "modules/rf/rf_utils.h"
#include <globals.h>

uint32_t rfRxCallback(cmd *c) {
    Command cmd(c);

    Argument rawArg = cmd.getArgument("raw");
    Argument freqArg = cmd.getArgument("frequency");
    bool raw = rawArg.isSet();
    String strFreq = freqArg.getValue();

    float frequency = strFreq.toFloat();
    frequency /= 1000000; // passed as a long int (e.g. 433920000)

    Serial.print("frequency: ");
    Serial.println(frequency);

    String r = "";
    if (raw) {
        r = RCSwitch_Read(frequency, 10, true); // true -> raw mode
    } else {
        r = RCSwitch_Read(frequency, 10, false); // false -> decoded mode
    }

    if (r.length() == 0) return false;

    Serial.println(r);
    return true;
}

uint32_t rfTxCallback(cmd *c) {
    // flipperzero-like cmd  https://docs.flipper.net/development/cli/#wLVht
    // e.g. subghz tx 0000000000200001 868250000 403 10  //
    // https://forum.flipper.net/t/friedland-libra-48249sl-wireless-doorbell-request/4528/20
    //                {hex_key}     {frequency} {te} {count}
    // subghz tx 445533 433920000 174 10

    Command cmd(c);

    Argument keyArg = cmd.getArgument("key");
    Argument freqArg = cmd.getArgument("frequency");
    Argument teArg = cmd.getArgument("te");
    Argument cntArg = cmd.getArgument("count");
    String strKey = keyArg.getValue();
    String strFrequency = freqArg.getValue();
    String strTe = teArg.getValue();
    String strCount = cntArg.getValue();

    uint64_t key = std::stoull(strKey.c_str(), nullptr, 16);
    unsigned long frequency = std::stoul(strFrequency.c_str());
    unsigned int te = std::stoul(strTe.c_str());
    unsigned int count = std::stoul(strCount.c_str());

    unsigned int bits = 24; // TODO: compute from key

    // check valid frequency and init the rf module
    if (!initRfModule("tx", float(frequency / 1000000.0))) return false;

    RCSwitch_send(key, bits, te, 1, count);
    deinitRfModule();
    return true;
}

uint32_t rfScanCallback(cmd *c) {
    // subghz scan 433 434

    Command cmd(c);

    Argument startArg = cmd.getArgument("start_frequency");
    Argument stopArg = cmd.getArgument("stop_frequency");
    String startFreqStr = startArg.getValue();
    String stopFreqStr = stopArg.getValue();

    float startFreq = startFreqStr.toFloat();
    float stopFreq = stopFreqStr.toFloat();

    if (startFreq == 0 || stopFreq == 0) {
        Serial.println("Invalid frequency range: " + String(startFreq) + " - " + String(stopFreq));
        return false;
    }

    // passed as a long int (e.g. 433920000)
    startFreq /= 1000000;
    stopFreq /= 1000000;

    rf_scan(startFreq, stopFreq, 10 * 1000); // 10s timeout
    return true;
}

uint32_t rfTxFileCallback(cmd *c) {
    // example: subghz tx_from_file plug1_on.sub

    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();

    if (filepath.indexOf(".sub") == -1) {
        Serial.println("Invalid file");
        return false;
    }

    if (!filepath.startsWith("/")) filepath = "/" + filepath;

    FS *fs;
    if (!getFsStorage(fs)) return false;

    if (!(*fs).exists(filepath)) {
        Serial.println("File does not exist");
        return false;
    }

    return txSubFile(fs, filepath);
}

uint32_t rfTxBufferCallback(cmd *c) {
    if (!(_setupPsramFs())) return false;

    char *txt = _readFileFromSerial();
    String tmpfilepath = "/tmpramfile"; // TODO: Change to use char *txt directly
    File f = PSRamFS.open(tmpfilepath, FILE_WRITE);
    if (!f) return false;

    f.write((const uint8_t *)txt, strlen(txt));
    f.close();
    free(txt);

    bool r = txSubFile(&PSRamFS, tmpfilepath);
    PSRamFS.remove(tmpfilepath);

    return r;
}

uint32_t rfSendCallback(cmd *c) {
    // tasmota json command  https://tasmota.github.io/docs/Tasmota-IR/#sending-ir-commands
    // e.g. RfSend {\"Data\":\"0x447503\",\"Bits\":24,\"Protocol\":1,\"Pulse\":174,\"Repeat\":10}  // on
    // e.g. RfSend {\"Data\":\"0x44750C\",\"Bits\":24,\"Protocol\":1,\"Pulse\":174,\"Repeat\":10}  // off

    Command cmd(c);

    Argument arg = cmd.getArgument(0);
    String command = arg.getValue();

    cJSON *root = cJSON_Parse(command.c_str());
    if (root == NULL) {
        Serial.println("This is NOT json format");
        return false;
    }
    unsigned int bits = 32; // defaults to 32 bits
    const char *dataStr = "";
    int protocol = 1; // defaults to 1
    int pulse = 0;    // 0 leave the library use the default value depending on protocol
    int repeat = 10;

    cJSON *protocolItem = cJSON_GetObjectItem(root, "protocol");
    cJSON *dataItem = cJSON_GetObjectItem(root, "data");
    cJSON *bitsItem = cJSON_GetObjectItem(root, "bits");
    cJSON *pulseItem = cJSON_GetObjectItem(root, "pulse");
    cJSON *repeatItem = cJSON_GetObjectItem(root, "repeat");

    if (protocolItem && cJSON_IsNumber(protocolItem)) protocol = protocolItem->valueint;
    if (bitsItem && cJSON_IsNumber(bitsItem)) bits = bitsItem->valueint;
    if (pulseItem && cJSON_IsNumber(pulseItem)) pulse = pulseItem->valueint;
    if (repeatItem && cJSON_IsNumber(repeatItem)) repeat = repeatItem->valueint;
    if (dataItem && cJSON_IsString(dataItem)) {
        dataStr = dataItem->valuestring;
    } else {
        Serial.println("Missing or invalid data to send");
        cJSON_Delete(root);
        return false;
    }

    // String dataStr = cmd_str.substring(36, 36+8);
    uint64_t data = strtoul(dataStr, nullptr, 16);
    // Serial.println(dataStr);
    // SerialPrintHexString(data);
    // Serial.println(bits);

    if (!initRfModule("tx")) return false;

    RCSwitch_send(data, bits, pulse, protocol, repeat);

    cJSON_Delete(root);
    return true;
}

void createRfRxCommand(Command *rfCmd) {
    Command cmd = rfCmd->addCommand("rx", rfRxCallback);
    cmd.addPosArg("frequency", String(bruceConfig.rfFreq).c_str());
    cmd.addFlagArg("raw");
}

void createRfTxCommand(Command *rfCmd) {
    Command cmd = rfCmd->addCommand("tx", rfTxCallback);
    cmd.addPosArg("key", "0");
    cmd.addPosArg("frequency", "433920000");
    cmd.addPosArg("te", "0");
    cmd.addPosArg("count", "10");
}

void createRfScanCommand(Command *rfCmd) {
    Command cmd = rfCmd->addCommand("scan", rfScanCallback);
    cmd.addPosArg("start_frequency");
    cmd.addPosArg("stop_frequency");
}

void createRfTxFileCommand(Command *rfCmd) {
    Command cmd = rfCmd->addCommand("tx_from_file", rfTxFileCallback);
    cmd.addPosArg("filepath");
}

void createRfTxBufferCommand(Command *rfCmd) {
    Command cmd = rfCmd->addCommand("tx_from_buffer", rfTxBufferCallback);
}

void createRfSendCommand(Command *rfCmd) { Command cmd = rfCmd->addSingleArgCmd("send", rfSendCallback); }

void createRfCommands(SimpleCLI *cli) {
    Command cmd = cli->addCompositeCmd("rf,subghz");

    createRfRxCommand(&cmd);
    createRfTxCommand(&cmd);
    createRfScanCommand(&cmd);
    createRfTxFileCommand(&cmd);
    createRfTxBufferCommand(&cmd);
    createRfSendCommand(&cmd);
}
