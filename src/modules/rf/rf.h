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