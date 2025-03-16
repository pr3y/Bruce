#include "ir_commands.h"
#include "cJSON.h"
#include "core/sd_functions.h"
#include "helpers.h"
#include "modules/ir/custom_ir.h"
#include "modules/ir/ir_read.h"
#include <globals.h>

uint32_t irCallback(cmd *c) {
    Serial.println("Turning off IR LED");
    digitalWrite(bruceConfig.irTx, LED_OFF);
    return true;
}

uint32_t irRxCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("raw");
    bool raw = arg.isSet();

    IrRead *i = NULL; // avoid calling the constructor here

    if (raw) {
        i = new IrRead(true, true); // true -> headless mode, true = raw mode
    } else {
        i = new IrRead(true); // true -> headless mode
    }

    Serial.println("Waiting for signal...");
    String r = i->loop_headless(10); // 10s timeout
    if (r.length() == 0) return false;

    Serial.println(r);
    delete i;
    return true;
}

uint32_t irTxCallback(cmd *c) {
    // usage: ir tx <protocol> <address without spaces> <command without spaces>
    // e.g. ir tx NEC 04000000 08000000

    Command cmd(c);

    Argument prtcArg = cmd.getArgument("protocol");
    Argument addrArg = cmd.getArgument("address");
    Argument cmdArg = cmd.getArgument("command");
    String protocol = prtcArg.getValue();
    String address = addrArg.getValue();
    String command = cmdArg.getValue();
    protocol.trim();
    address.trim();
    command.trim();

    if (address.length() != 8 || command.length() != 8) {
        Serial.println("Address and command must be 8 characters long");
        return false;
    }

    IRCode code;
    code.type = "parsed";
    code.protocol = protocol;
    code.address = address;
    code.command = command;

    sendIRCommand(&code);
    return true;
}

uint32_t irTxRawCallback(cmd *c) {
    // usage: ir tx_raw <frequency> <samples>

    Command cmd(c);

    Argument freqArg = cmd.getArgument("frequency");
    Argument samplesArg = cmd.getArgument("samples");
    String freqStr = freqArg.getValue();
    String samples = samplesArg.getValue();
    freqStr.trim();
    samples.trim();

    uint32_t frequency = freqStr.toInt();

    if (frequency == 0) {
        Serial.println("Invalid frequency: " + String(frequency));
        return false;
    }

    if (samples.length() == 0) {
        Serial.println("Missing data samples");
        return false;
    }

    IRCode code;
    code.type = "raw";
    code.frequency = frequency;
    code.data = samples;

    sendIRCommand(&code);
    return true;
}

uint32_t irTxFileCallback(cmd *c) {
    // example: ir tx_from_file LG_AKB72915206_power.ir

    Command cmd(c);

    Argument arg = cmd.getArgument("filepath");
    String filepath = arg.getValue();
    filepath.trim();

    if (filepath.indexOf(".ir") == -1) {
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

    return txIrFile(fs, filepath);
}

uint32_t irTxBufferCallback(cmd *c) {
    if (!(_setupPsramFs())) return false;

    char *txt = _readFileFromSerial();
    String tmpfilepath = "/tmpramfile"; // TODO: Change to use char *txt directly
    File f = PSRamFS.open(tmpfilepath, FILE_WRITE);
    if (!f) return false;

    f.write((const uint8_t *)txt, strlen(txt));
    f.close();
    free(txt);

    bool r = txIrFile(&PSRamFS, tmpfilepath);
    PSRamFS.remove(tmpfilepath);

    return r;
}

uint32_t irSendCallback(cmd *c) {
    // tasmota json command  https://tasmota.github.io/docs/Tasmota-IR/#sending-ir-commands
    // e.g. IRSend {\"Protocol\":\"NEC\",\"Bits\":32,\"Data\":\"0x20DF10EF\"}
    // TODO: rewrite using ArduinoJson parser?
    // TODO: decode "data" into "address, command" and use existing "send*Command" funcs

    Command cmd(c);

    Argument arg = cmd.getArgument(0);
    String command = arg.getValue();
    command.trim();

    cJSON *root = cJSON_Parse(command.c_str());
    if (root == NULL) {
        Serial.println("This is NOT json format");
        return false;
    }
    uint16_t bits = 32; // defaults to 32 bits
    const char *dataStr = "";
    String protocolStr = "nec"; // defaults to NEC protocol

    cJSON *protocolItem = cJSON_GetObjectItem(root, "protocol");
    cJSON *dataItem = cJSON_GetObjectItem(root, "data");
    cJSON *bitsItem = cJSON_GetObjectItem(root, "bits");

    if (protocolItem && cJSON_IsString(protocolItem)) {
        protocolStr = protocolItem->valuestring;
    } else {
        Serial.println("Missing or invalid protocol to send");
        return false;
    }
    if (bitsItem && cJSON_IsNumber(bitsItem)) bits = bitsItem->valueint;
    if (dataItem && cJSON_IsString(dataItem)) {
        dataStr = dataItem->valuestring;
    } else {
        Serial.println("Missing or invalid data to send");
        return false;
    }
    // String dataStr = cmd_str.substring(36, 36+8);
    uint64_t data = strtoul(dataStr, nullptr, 16);
    // Serial.println(dataStr);
    // SerialPrintHexString(data);
    // Serial.println(bits);
    // Serial.println(protocolItem->valuestring);

    cJSON_Delete(root);

    /*if(protocolStr == "nec"){
      // sendNEC(uint64_t data, uint16_t nbits, uint16_t repeat)
      irsend.sendNEC(data, bits, 10);
      return true;
    }
    */

    return sendDecodedCommand(protocolStr, dataStr, bits);
}

void createIrRxCommand(Command *irCmd) {
    Command cmd = irCmd->addCommand("rx", irRxCallback);
    cmd.addFlagArg("raw");
}

void createIrTxCommand(Command *irCmd) {
    Command cmd = irCmd->addCommand("tx", irTxCallback);
    cmd.addPositionalArgument("protocol");
    cmd.addPositionalArgument("address");
    cmd.addPositionalArgument("command");
}

void createIrTxRawCommand(Command *irCmd) {
    Command cmd = irCmd->addCommand("tx_raw", irTxRawCallback);
    cmd.addPositionalArgument("frequency");
    cmd.addPositionalArgument("samples");
}

void createIrTxFileCommand(Command *irCmd) {
    Command cmd = irCmd->addCommand("tx_from_file", irTxFileCallback);
    cmd.addPositionalArgument("filepath");
}

void createIrTxBufferCommand(Command *irCmd) { Command cmd = irCmd->addCommand("tx_from_buffer", irTxBufferCallback); }

void createIrSendCommand(Command *irCmd) { Command cmd = irCmd->addSingleArgCmd("send", irSendCallback); }

void createIrCommands(SimpleCLI *cli) {
    Command cmd = cli->addCompositeCmd("ir", irCallback);

    createIrRxCommand(&cmd);
    createIrTxCommand(&cmd);
    createIrTxRawCommand(&cmd);
    createIrTxFileCommand(&cmd);
    createIrTxBufferCommand(&cmd);
    createIrSendCommand(&cmd);
}
