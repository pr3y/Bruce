#include "protocol_items.h"

#include <furi.h>
#include <string.h>

#include "../defines.h"

#define TAG "PPCatalog"

#define PROTOPIRATE_CC1101_REG_MDMCFG2        0x12U
#define PROTOPIRATE_CC1101_MOD_FORMAT_MASK    0x70U
#define PROTOPIRATE_CC1101_MOD_FORMAT_2FSK    0x00U
#define PROTOPIRATE_CC1101_MOD_FORMAT_GFSK    0x10U
#define PROTOPIRATE_CC1101_MOD_FORMAT_ASK_OOK 0x30U
#define PROTOPIRATE_CC1101_MOD_FORMAT_4FSK    0x40U
#define PROTOPIRATE_CC1101_MOD_FORMAT_MSK     0x70U
#define PROTOPIRATE_VAG_FREQUENCY_MIN         434190000UL
#define PROTOPIRATE_VAG_FREQUENCY_MAX         434450000UL

#define PROTOPIRATE_COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

#ifdef ENABLE_EMULATE_FEATURE
#define PROTOPIRATE_TX_KEY(key) key
#else
#define PROTOPIRATE_TX_KEY(key) NULL
#endif

typedef enum {
    ProtoPirateProtocolCatalogModulationAM = 0,
    ProtoPirateProtocolCatalogModulationFM,
} ProtoPirateProtocolCatalogModulation;

typedef struct {
    const char* alias;
    const char* canonical_name;
} ProtoPirateProtocolCatalogAlias;

static const ProtoPirateProtocolCatalogEntry protopirate_protocol_catalog[] = {
    {"Chrysler V0", ProtoPirateProtocolCatalogRouteAMDefault, PROTOPIRATE_TX_KEY("chrysler_v0")},
    {"Fiat V0", ProtoPirateProtocolCatalogRouteAMDefault, PROTOPIRATE_TX_KEY("fiat_v0")},
    {"Fiat V1", ProtoPirateProtocolCatalogRouteAMDefault, PROTOPIRATE_TX_KEY("fiat_v1")},
    {"Fiat V2", ProtoPirateProtocolCatalogRouteAMDefault, NULL},
    {"Ford V0", ProtoPirateProtocolCatalogRouteAMDefault, PROTOPIRATE_TX_KEY("ford_v0")},
    {"Ford V1", ProtoPirateProtocolCatalogRouteFMF4, PROTOPIRATE_TX_KEY("ford_v1")},
    {"Ford V2", ProtoPirateProtocolCatalogRouteFMF4, PROTOPIRATE_TX_KEY("ford_v2")},
    {"Ford V3", ProtoPirateProtocolCatalogRouteFMF4, NULL},
    {"Honda Static", ProtoPirateProtocolCatalogRouteFMHonda1, PROTOPIRATE_TX_KEY("honda_static")},
    {"Honda V1", ProtoPirateProtocolCatalogRouteAMDefault, PROTOPIRATE_TX_KEY("honda_v1")},
    {"Kia V0", ProtoPirateProtocolCatalogRouteFMDefault, PROTOPIRATE_TX_KEY("kia_v0")},
    {"Kia V1", ProtoPirateProtocolCatalogRouteAMDefault, PROTOPIRATE_TX_KEY("kia_v1")},
    {"Kia V2", ProtoPirateProtocolCatalogRouteAMDefault, PROTOPIRATE_TX_KEY("kia_v2")},
    {"Kia V3/V4", ProtoPirateProtocolCatalogRouteFMDefault, PROTOPIRATE_TX_KEY("kia_v3_v4")},
    {"Kia V5", ProtoPirateProtocolCatalogRouteFMDefault, PROTOPIRATE_TX_KEY("kia_v5")},
    {"Kia V6", ProtoPirateProtocolCatalogRouteFMDefault, PROTOPIRATE_TX_KEY("kia_v6")},
    {"Kia V7", ProtoPirateProtocolCatalogRouteFMDefault, PROTOPIRATE_TX_KEY("kia_v7")},
    {"Honda V2", ProtoPirateProtocolCatalogRouteFMF4, PROTOPIRATE_TX_KEY("honda_v2")},
    {"Mazda V0", ProtoPirateProtocolCatalogRouteByModulation, PROTOPIRATE_TX_KEY("mazda_v0")},
    {"PSA", ProtoPirateProtocolCatalogRouteByModulation, PROTOPIRATE_TX_KEY("psa")},
    {"Renault V0", ProtoPirateProtocolCatalogRouteAMDefault, PROTOPIRATE_TX_KEY("renault_v0")},
    {"Renault V1", ProtoPirateProtocolCatalogRouteByModulation, PROTOPIRATE_TX_KEY("renault_v1")},
    {"Scher-Khan", ProtoPirateProtocolCatalogRouteFMDefault, NULL},
    {"Star Line", ProtoPirateProtocolCatalogRouteAMDefault, PROTOPIRATE_TX_KEY("star_line")},
    {"Subaru", ProtoPirateProtocolCatalogRouteAMDefault, PROTOPIRATE_TX_KEY("subaru")},
    {"VAG", ProtoPirateProtocolCatalogRouteAMVag, PROTOPIRATE_TX_KEY("vag")},
};

static const ProtoPirateProtocolCatalogAlias protopirate_protocol_catalog_aliases[] = {
    {"StarLine", "Star Line"},
    {"Kia V3", "Kia V3/V4"},
    {"Kia V4", "Kia V3/V4"},
    {"KIA/HYU V3", "Kia V3/V4"},
    {"KIA/HYU V4", "Kia V3/V4"},
    {"Suzuki", "Kia V0"},
    {"Suzuki V0", "Kia V0"},
    {"Honda V0", "Kia V0"},
    {"Mitsu V0", "Kia V0"},
    {"Mitsu v0", "Kia V0"},
    {"Mitsubishi V0", "Kia V0"},
    {"Mitsubishi v0", "Kia V0"},
    {"Land Rover V0", "Honda V2"},
    {"VW", "VAG"},
};

static bool protopirate_catalog_string_equal(const char* a, const char* b) {
    return a && b && strcmp(a, b) == 0;
}

static bool protopirate_catalog_string_contains(const char* haystack, const char* needle) {
    return haystack && needle && strstr(haystack, needle) != NULL;
}

static const ProtoPirateProtocolCatalogEntry*
    protopirate_protocol_catalog_find_canonical(const char* canonical_name) {
    if(!canonical_name || canonical_name[0] == '\0') {
        return NULL;
    }
    for(size_t i = 0; i < PROTOPIRATE_COUNT_OF(protopirate_protocol_catalog); i++) {
        if(protopirate_catalog_string_equal(
               canonical_name, protopirate_protocol_catalog[i].canonical_name)) {
            return &protopirate_protocol_catalog[i];
        }
    }
    return NULL;
}

static const char* protopirate_protocol_catalog_alias_to_canonical(const char* protocol_name) {
    if(!protocol_name || protocol_name[0] == '\0') {
        return NULL;
    }
    for(size_t i = 0; i < PROTOPIRATE_COUNT_OF(protopirate_protocol_catalog_aliases); i++) {
        if(protopirate_catalog_string_equal(
               protocol_name, protopirate_protocol_catalog_aliases[i].alias)) {
            return protopirate_protocol_catalog_aliases[i].canonical_name;
        }
    }
    return NULL;
}

static const ProtoPirateProtocolCatalogEntry*
    protopirate_protocol_catalog_find_substring(const char* protocol_name) {
    if(!protocol_name || protocol_name[0] == '\0') {
        return NULL;
    }
    for(size_t i = 0; i < PROTOPIRATE_COUNT_OF(protopirate_protocol_catalog); i++) {
        if(protopirate_catalog_string_contains(
               protocol_name, protopirate_protocol_catalog[i].canonical_name)) {
            return &protopirate_protocol_catalog[i];
        }
    }
    for(size_t i = 0; i < PROTOPIRATE_COUNT_OF(protopirate_protocol_catalog_aliases); i++) {
        if(protopirate_catalog_string_contains(
               protocol_name, protopirate_protocol_catalog_aliases[i].alias)) {
            return protopirate_protocol_catalog_find_canonical(
                protopirate_protocol_catalog_aliases[i].canonical_name);
        }
    }
    return NULL;
}

const ProtoPirateProtocolCatalogEntry*
    protopirate_protocol_catalog_find(const char* protocol_name) {
    if(!protocol_name || protocol_name[0] == '\0') {
        return NULL;
    }
    const ProtoPirateProtocolCatalogEntry* entry =
        protopirate_protocol_catalog_find_canonical(protocol_name);
    if(entry) {
        return entry;
    }

    const char* canonical_name = protopirate_protocol_catalog_alias_to_canonical(protocol_name);
    if(canonical_name) {
        return protopirate_protocol_catalog_find_canonical(canonical_name);
    }
    return NULL;
}

const char* protopirate_protocol_catalog_canonical_name(const char* protocol_name) {
    const ProtoPirateProtocolCatalogEntry* entry =
        protopirate_protocol_catalog_find(protocol_name);
    return entry ? entry->canonical_name : protocol_name;
}

bool protopirate_protocol_catalog_can_tx(const char* protocol_name) {
    return protopirate_protocol_catalog_tx_key(protocol_name) != NULL;
}

bool protopirate_protocol_catalog_offers_bruteforce(const char* protocol_name) {
    const char* canonical = protopirate_protocol_catalog_canonical_name(protocol_name);
    return protopirate_catalog_string_equal(canonical, "PSA") ||
           protopirate_catalog_string_equal(canonical, "Renault V1");
}

const char* protopirate_protocol_catalog_tx_key(const char* protocol_name) {
    const ProtoPirateProtocolCatalogEntry* entry =
        protopirate_protocol_catalog_find(protocol_name);
    return entry ? entry->tx_key : NULL;
}

const char*
    protopirate_protocol_catalog_display_name(const char* protocol_name, uint32_t protocol_type) {
    if(!protocol_name) {
        return NULL;
    }

    if(protopirate_catalog_string_equal(protocol_name, "Suzuki") ||
       protopirate_catalog_string_equal(protocol_name, "Suzuki V0")) {
        return "Suzuki V0";
    }
    if(protopirate_catalog_string_equal(protocol_name, "Honda V0")) {
        return "Honda V0";
    }
    if(protopirate_catalog_string_equal(protocol_name, "Mitsu V0") ||
       protopirate_catalog_string_equal(protocol_name, "Mitsu v0") ||
       protopirate_catalog_string_equal(protocol_name, "Mitsubishi V0") ||
       protopirate_catalog_string_equal(protocol_name, "Mitsubishi v0")) {
        return "Mitsubishi V0";
    }

    const char* canonical_name = protopirate_protocol_catalog_canonical_name(protocol_name);
    if(protopirate_catalog_string_equal(canonical_name, "Kia V0")) {
        if(protocol_type == 2U) {
            return "Suzuki V0";
        }
        if(protocol_type == 3U) {
            return "Honda V0";
        }
        if(protocol_type == 4U) {
            return "Mitsubishi V0";
        }
    }

    return canonical_name;
}

static bool protopirate_preset_try_get_register(
    const uint8_t* preset_data,
    size_t preset_data_size,
    uint8_t reg,
    uint8_t* value) {
    if(!preset_data || !value || (preset_data_size < 2U)) {
        return false;
    }

    for(size_t i = 0; i + 1U < preset_data_size; i += 2U) {
        const uint8_t address = preset_data[i];
        const uint8_t data = preset_data[i + 1U];

        if((address == 0x00U) && (data == 0x00U)) {
            break;
        }

        if(address == reg) {
            *value = data;
            return true;
        }
    }

    return false;
}

static ProtoPirateProtocolCatalogModulation protopirate_protocol_catalog_get_modulation(
    const uint8_t* preset_data,
    size_t preset_data_size) {
    uint8_t mdmcfg2 = 0U;

    if(!protopirate_preset_try_get_register(
           preset_data, preset_data_size, PROTOPIRATE_CC1101_REG_MDMCFG2, &mdmcfg2)) {
        FURI_LOG_W(TAG, "Preset missing MDMCFG2, defaulting to AM registry");
        return ProtoPirateProtocolCatalogModulationAM;
    }

    switch(mdmcfg2 & PROTOPIRATE_CC1101_MOD_FORMAT_MASK) {
    case PROTOPIRATE_CC1101_MOD_FORMAT_ASK_OOK:
        return ProtoPirateProtocolCatalogModulationAM;
    case PROTOPIRATE_CC1101_MOD_FORMAT_2FSK:
    case PROTOPIRATE_CC1101_MOD_FORMAT_GFSK:
    case PROTOPIRATE_CC1101_MOD_FORMAT_4FSK:
    case PROTOPIRATE_CC1101_MOD_FORMAT_MSK:
        return ProtoPirateProtocolCatalogModulationFM;
    default:
        FURI_LOG_W(TAG, "Unknown MDMCFG2 0x%02X, defaulting to AM registry", mdmcfg2);
        return ProtoPirateProtocolCatalogModulationAM;
    }
}

static bool protopirate_frequency_in_vag_band(uint32_t frequency) {
    return frequency >= PROTOPIRATE_VAG_FREQUENCY_MIN &&
           frequency <= PROTOPIRATE_VAG_FREQUENCY_MAX;
}

static ProtoPirateProtocolRegistryRoute protopirate_catalog_route_from_policy(
    ProtoPirateProtocolCatalogRoutePolicy policy,
    ProtoPirateProtocolCatalogModulation modulation) {
    switch(policy) {
    case ProtoPirateProtocolCatalogRouteAMVag:
        return ProtoPirateProtocolRegistryRouteAMVag;
    case ProtoPirateProtocolCatalogRouteFMDefault:
        return ProtoPirateProtocolRegistryRouteFMDefault;
    case ProtoPirateProtocolCatalogRouteFMF4:
        return ProtoPirateProtocolRegistryRouteFMF4;
    case ProtoPirateProtocolCatalogRouteFMHonda1:
        return ProtoPirateProtocolRegistryRouteFMHonda1;
    case ProtoPirateProtocolCatalogRouteByModulation:
        return (modulation == ProtoPirateProtocolCatalogModulationAM) ?
                   ProtoPirateProtocolRegistryRouteAMDefault :
                   ProtoPirateProtocolRegistryRouteFMDefault;
    case ProtoPirateProtocolCatalogRouteAMDefault:
    default:
        return ProtoPirateProtocolRegistryRouteAMDefault;
    }
}

ProtoPirateProtocolRegistryRoute protopirate_protocol_catalog_get_route(
    const char* preset_name,
    uint32_t frequency,
    const uint8_t* preset_data,
    size_t preset_data_size,
    const char* protocol_name) {
    const ProtoPirateProtocolCatalogEntry* entry =
        protopirate_protocol_catalog_find(protocol_name);
    if(!entry) {
        entry = protopirate_protocol_catalog_find_substring(protocol_name);
    }

    if(entry && entry->route_policy != ProtoPirateProtocolCatalogRouteByModulation) {
        return protopirate_catalog_route_from_policy(
            entry->route_policy, ProtoPirateProtocolCatalogModulationAM);
    }

    const ProtoPirateProtocolCatalogModulation modulation =
        protopirate_protocol_catalog_get_modulation(preset_data, preset_data_size);

    if(entry) {
        return protopirate_catalog_route_from_policy(entry->route_policy, modulation);
    }

    if(modulation == ProtoPirateProtocolCatalogModulationAM) {
        if(protopirate_frequency_in_vag_band(frequency)) {
            return ProtoPirateProtocolRegistryRouteAMVag;
        }
        return ProtoPirateProtocolRegistryRouteAMDefault;
    }

    if(protopirate_catalog_string_contains(preset_name, "F4")) {
        return ProtoPirateProtocolRegistryRouteFMF4;
    }
    if(protopirate_catalog_string_contains(preset_name, "Honda1") ||
       protopirate_catalog_string_contains(preset_name, "Honda 1")) {
        return ProtoPirateProtocolRegistryRouteFMHonda1;
    }
    return ProtoPirateProtocolRegistryRouteFMDefault;
}

ProtoPirateProtocolRegistryRoute protopirate_get_protocol_registry_route(
    const char* preset_name,
    uint32_t frequency,
    const uint8_t* preset_data,
    size_t preset_data_size,
    const char* protocol_name) {
    return protopirate_protocol_catalog_get_route(
        preset_name, frequency, preset_data, preset_data_size, protocol_name);
}

const char* protopirate_get_protocol_registry_route_name(ProtoPirateProtocolRegistryRoute route) {
    switch(route) {
    case ProtoPirateProtocolRegistryRouteAMVag:
        return "AM_VAG";
    case ProtoPirateProtocolRegistryRouteFMDefault:
        return "FM_DEFAULT";
    case ProtoPirateProtocolRegistryRouteFMF4:
        return "FM_F4";
    case ProtoPirateProtocolRegistryRouteFMHonda1:
        return "FM_HONDA1";
    case ProtoPirateProtocolRegistryRouteAMDefault:
    default:
        return "AM_DEFAULT";
    }
}
