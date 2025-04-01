#include "wifi_commands.h"
#include "core/wifi/webInterface.h"
#include "core/wifi/wifi_common.h" //to return MAC addr
#include <globals.h>

uint32_t wifiCallback(cmd *c) {
    Command cmd(c);
    Argument statusArg = cmd.getArgument("status");
    String status = statusArg.getValue();
    status.trim();

    if (status == "off") {
        wifiDisconnect();
        return true;
    }
    // else if (status == "on") {
    //     if (wifiConnected) {
    //         Serial.println("Wifi already connected");
    //         return true;
    //     }
    //     connectToWifi();
    //     return true;
    // }
    else {
        Serial.println("Invalid status: " + status);
        return false;
    }
}

uint32_t webuiCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("noAp");
    bool noAp = arg.isSet();

    Serial.println("Starting Web UI " + !noAp ? "AP" : "STA");
    Serial.println("Press ESC to quit");
    startWebUi(!noAp); // MEMO: will quit when check(EscPress)

    return true;
}

void createWifiCommands(SimpleCLI *cli) {
    Command webuiCmd = cli->addCommand("webui", webuiCallback);
    webuiCmd.addFlagArg("noAp");

    Command wifiCmd = cli->addCommand("wifi", wifiCallback);
    wifiCmd.addPosArg("status");
}
