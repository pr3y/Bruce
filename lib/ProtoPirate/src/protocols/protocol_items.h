#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    ProtoPirateProtocolRegistryRouteAMDefault = 0,
    ProtoPirateProtocolRegistryRouteAMVag,
    ProtoPirateProtocolRegistryRouteFMDefault,
    ProtoPirateProtocolRegistryRouteFMF4,
    ProtoPirateProtocolRegistryRouteFMHonda1,
} ProtoPirateProtocolRegistryRoute;

typedef enum {
    ProtoPirateProtocolCatalogRouteAMDefault = 0,
    ProtoPirateProtocolCatalogRouteAMVag,
    ProtoPirateProtocolCatalogRouteFMDefault,
    ProtoPirateProtocolCatalogRouteFMF4,
    ProtoPirateProtocolCatalogRouteFMHonda1,
    ProtoPirateProtocolCatalogRouteByModulation,
} ProtoPirateProtocolCatalogRoutePolicy;

typedef struct {
    const char* canonical_name;
    ProtoPirateProtocolCatalogRoutePolicy route_policy;
    const char* tx_key;
} ProtoPirateProtocolCatalogEntry;

const ProtoPirateProtocolCatalogEntry*
    protopirate_protocol_catalog_find(const char* protocol_name);

const char* protopirate_protocol_catalog_canonical_name(const char* protocol_name);

bool protopirate_protocol_catalog_can_tx(const char* protocol_name);

bool protopirate_protocol_catalog_offers_bruteforce(const char* protocol_name);

const char* protopirate_protocol_catalog_tx_key(const char* protocol_name);

const char*
    protopirate_protocol_catalog_display_name(const char* protocol_name, uint32_t protocol_type);

ProtoPirateProtocolRegistryRoute protopirate_protocol_catalog_get_route(
    const char* preset_name,
    uint32_t frequency,
    const uint8_t* preset_data,
    size_t preset_data_size,
    const char* protocol_name);

ProtoPirateProtocolRegistryRoute protopirate_get_protocol_registry_route(
    const char* preset_name,
    uint32_t frequency,
    const uint8_t* preset_data,
    size_t preset_data_size,
    const char* protocol_name);

const char* protopirate_get_protocol_registry_route_name(ProtoPirateProtocolRegistryRoute route);
