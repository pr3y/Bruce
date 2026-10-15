#pragma once

#include <lib/subghz/types.h>
#include "protocol_items.h"

#define PROTOPIRATE_PROTOCOL_PLUGIN_APP_ID      "protopirate_protocol_plugins"
#define PROTOPIRATE_PROTOCOL_PLUGIN_API_VERSION ((uint32_t)sizeof(ProtoPirateProtocolPlugin))

//This is the maximum length of all protocols. It is currently used so the Save Dialogs in Sub Decode
// and receiver can allocate enough, but not too much memory to store a buffer with the protocol name included.
#define PROTOPIRATE_PROTOCOL_NAME_MAX \
    15 //"Mitsubishi V0" is longest name at present, leave a couple extra bytes.
typedef enum {
    ProtoPirateProtocolPluginKindRx = 0,
    ProtoPirateProtocolPluginKindTx,
} ProtoPirateProtocolPluginKind;

typedef struct {
    const char* plugin_name;
    ProtoPirateProtocolPluginKind kind;
    ProtoPirateProtocolRegistryRoute route;
    const char* protocol_name;
    const SubGhzProtocolRegistry* registry;
    void (*release)(void);
} ProtoPirateProtocolPlugin;
