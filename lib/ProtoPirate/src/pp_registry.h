#include "protocols/chrysler_v0.h"
#include "protocols/fiat_v0.h"
#include "protocols/fiat_v1.h"
#include "protocols/fiat_v2.h"
#include "protocols/ford_v0.h"
#include "protocols/ford_v1.h"
#include "protocols/ford_v2.h"
#include "protocols/ford_v3.h"
#include "protocols/honda_static.h"
#include "protocols/honda_v1.h"
#include "protocols/honda_v2.h"
#include "protocols/kia_v0.h"
#include "protocols/kia_v1.h"
#include "protocols/kia_v2.h"
#include "protocols/kia_v3_v4.h"
#include "protocols/kia_v5.h"
#include "protocols/kia_v6.h"
#include "protocols/kia_v7.h"
#include "protocols/mazda_v0.h"
#include "protocols/psa.h"
#include "protocols/renault_v0.h"
#include "protocols/renault_v1.h"
#include "protocols/scher_khan.h"
#include "protocols/star_line.h"
#include "protocols/subaru.h"
#include "protocols/vag.h"
static const SubGhzProtocol *const protocols[] = {
    &chrysler_protocol_v0,
    &fiat_protocol_v0,
    &fiat_v1_protocol,
    &fiat_v2_protocol,
    &ford_protocol_v0,
    &ford_protocol_v1,
    &ford_protocol_v2,
    &ford_protocol_v3,
    &honda_static_protocol,
    &honda_v1_protocol,
    &honda_v2_protocol,
    &kia_protocol_v0,
    &kia_protocol_v1,
    &kia_protocol_v2,
    &kia_protocol_v3_v4,
    &kia_protocol_v5,
    &kia_protocol_v6,
    &kia_protocol_v7,
    &mazda_v0_protocol,
    &psa_protocol,
    &renault_v0_protocol,
    &renault_v1_protocol,
    &subghz_protocol_scher_khan,
    &subghz_protocol_star_line,
    &subaru_protocol,
    &vag_protocol,
};
