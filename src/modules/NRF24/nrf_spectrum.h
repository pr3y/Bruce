#pragma once
#include <RF24.h>

void nrf_spectrum(SPIClass *SSPI);

String scanChannels(SPIClass *SSPI, bool web = false);
