#include "core/display.h"
#include <globals.h>

// Configuração do personagem principal (tubarão)
extern int sharkX;
extern int sharkY;
extern int sharkSize;

// Configuração de pontuação
extern int score;

// Funções para manipulação do jogo
void drawShark();
// void drawFish(Fish &f);
void drawFish();
void moveShark();
// void moveFish(Fish &f);
void moveFish();
void checkCollisions();
void displayScore();
void shark_setup();
void shark_loop();
