// @IncursioHack - https://github.com/IncursioHack
#include <Wire.h>
#include <SD.h>
#include "PCA9554.h" // Biblioteca para PCA9554

// Define o endereço I2C do PCA9554PW
const int pca9554pw_address = 0x27;

// Cria um objeto PCA9554 com o endereço I2C do PCA9554PW
extern PCA9554 extIo1;

// Define os pinos para o receptor e transmissor
const int PCA9554RSX_PIN = 4;
const int PCA9554TRX_PIN = 0;

void rf_spectrum();
void rf_jammerIntermittent();
void rf_jammerFull();
void otherRFcodes();
bool txSubFile(FS *fs, String filepath);
bool RCSwitch_Read_Raw(float frequency=0, int max_loops=-1);
void RCSwitch_send(uint64_t data, unsigned int bits, int pulse=0, int protocol=1, int repeat=10);
void addToRecentCodes(struct RfCodes rfcode);
void sendRfCommand(struct RfCodes rfcode);
bool initRfModule(String mode="", float frequency=0);
void initCC1101once(SPIClass* SSPI);
void deinitRfModule();
