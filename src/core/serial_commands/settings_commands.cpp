#include "settings_commands.h"
#include "core/utils.h"
#include <globals.h>

uint32_t settingsCallback(cmd *c) {
    Command cmd(c);

    Argument setting_name_arg = cmd.getArgument("setting_name");
    Argument setting_value_arg = cmd.getArgument("setting_value");
    String setting_name = setting_name_arg.getValue();
    String setting_value = setting_value_arg.getValue();
    setting_name.trim();
    setting_value.trim();

    JsonDocument jsonDoc = bruceConfig.toJson();
    JsonObject setting = jsonDoc.as<JsonObject>();

    if (setting_name.length() == 0 && setting_value.length() == 0) {
        // no args, just prints current config
        serializeJsonPretty(jsonDoc, Serial);
        serialDevice->println("");
        return true;
    }

    if (setting[setting_name].isNull()) {
        serialDevice->println("Invalid field name: " + setting_name);
        return false;
    }

    if (setting_value.length() == 0) {
        serialDevice->print(setting_name + " = ");
        serialDevice->println(setting[setting_name].as<String>());
        return true;
    }

    // TODO: improve this logic and move to BruceConfig
    if (setting_name == "priColor") bruceConfig.setUiColor(setting_value.toInt());
    if (setting_name == "rot") bruceConfig.setRotation(setting_value.toInt());
    if (setting_name == "dimmerSet") bruceConfig.setDimmer(setting_value.toInt());
    if (setting_name == "bright") bruceConfig.setBright(setting_value.toInt());
    if (setting_name == "tmz") bruceConfig.setTmz(setting_value.toFloat());
    if (setting_name == "dst") {
        bruceConfig.setDST(setting_value.toInt());
        updateClockTimezone();
    }
    if (setting_name == "clock24hr") bruceConfig.setClock24Hr(setting_value.toInt());
    if (setting_name == "soundEnabled") bruceConfig.setSoundEnabled(setting_value.toInt());
    if (setting_name == "wifiAtStartup") bruceConfig.setWifiAtStartup(setting_value.toInt());
    if (setting_name == "webUI") {
        bruceConfig.setWebUICreds(
            setting_value.substring(0, setting_value.indexOf(",")),
            setting_value.substring(setting_value.indexOf(",") + 1)
        );
    }
    if (setting_name == "wifiAp") {
        bruceConfig.setWifiApCreds(
            setting_value.substring(0, setting_value.indexOf(",")),
            setting_value.substring(setting_value.indexOf(",") + 1)
        );
    }
    if (setting_name == "wifi") {
        bruceConfig.addWifiCredential(
            setting_value.substring(0, setting_value.indexOf(",")),
            setting_value.substring(setting_value.indexOf(",") + 1)
        );
    }
    if (setting_name == "bleName") bruceConfig.setBleName(setting_value);
    if (setting_name == "irTx") bruceConfig.setIrTxPin(setting_value.toInt());
    if (setting_name == "irTxRepeats")
        bruceConfig.setIrTxRepeats(static_cast<uint8_t>(setting_value.toInt()));
    if (setting_name == "irRx") bruceConfig.setIrRxPin(setting_value.toInt());
    if (setting_name == "rfTx") bruceConfig.setRfTxPin(setting_value.toInt());
    if (setting_name == "rfRx") bruceConfig.setRfRxPin(setting_value.toInt());
    if (setting_name == "rfModule") bruceConfig.setRfModule(static_cast<RFModules>(setting_value.toInt()));
    if (setting_name == "rfFreq" && setting_value.toFloat()) bruceConfig.setRfFreq(setting_value.toFloat());
    if (setting_name == "rfFxdFreq") bruceConfig.setRfFxdFreq(setting_value.toInt());
    if (setting_name == "rfScanRange") bruceConfig.setRfScanRange(setting_value.toInt());
    if (setting_name == "rfidModule")
        bruceConfig.setRfidModule(static_cast<RFIDModules>(setting_value.toInt()));
    if (setting_name == "wigleBasicToken") bruceConfig.setWigleBasicToken(setting_value);
    if (setting_name == "devMode") bruceConfig.setDevMode(setting_value.toInt());
    if (setting_name == "disabledMenus") bruceConfig.addDisabledMenu(setting_value);

    return true;
}

uint32_t factoryResetCallback(cmd *c) {
    bruceConfig.factoryReset();
    serialDevice->println("Factory reset done");
    return true;
}

void createSettingsCommands(SimpleCLI *cli) {
    cli->addCommand("factory_reset", factoryResetCallback);

    Command cmd = cli->addCommand("set/tings", settingsCallback);
    cmd.addPosArg("setting_name", "");
    cmd.addPosArg("setting_value", "");
}
