#pragma once
#include <furi.h>
typedef struct SubGhzProtocol SubGhzProtocol;
typedef struct SubGhzKeystore SubGhzKeystore;
typedef struct SubGhzEnvironment { SubGhzKeystore *keystore; } SubGhzEnvironment;
SubGhzKeystore *subghz_environment_get_keystore(SubGhzEnvironment *e);
