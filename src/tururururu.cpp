#include "mykeyboard.h"
#include "tururururu.h"
#include "globals.h"
#include "display.h"

// Configuração do personagem principal (tubarão)
int sharkX = 40;
int sharkY = 80;
int sharkSize = 10;

//Configuração dos peixes
struct Fish {
    int x, y, size;
};
Fish fish[5];  // Array de peixes

// Configuração de pontuação
int score = 0;

// Função para desenhar o tubarão
void drawShark() {
    tft.fillRect(sharkX, sharkY, sharkSize, sharkSize, TFT_BLUE);
}

// Função para desenhar peixes
void drawFish(Fish &f) {
    tft.fillRect(f.x, f.y, f.size, f.size, TFT_GREEN);
}

// Função para mover o tubarão
void moveShark() {

    if (checkPrevPress() || checkSelPress()) {
        sharkY -= 2;  // Move para cima
    }
    if (checkNextPress()) {
        sharkY += 2;  // Move para baixo
    }
    if (sharkY < 0) {
        sharkY = 0;
    }
    if (sharkY > tft.height() - sharkSize) {
        sharkY = tft.height() - sharkSize;
    }
}

// Função para mover peixes
void moveFish(Fish &f) {
    f.x -= 2;  // Move o peixe para a esquerda
    if (f.x < -10) {
        f.x = tft.width() + random(20, 100);
        f.y = random(10, tft.height() - 20);
    }
}

// Função para verificar colisões entre o tubarão e os peixes
void checkCollisions() {
    for (int i = 0; i < 5; i++) {
        if ((sharkX < fish[i].x + fish[i].size) && (sharkX + sharkSize > fish[i].x) &&
            (sharkY < fish[i].y + fish[i].size) && (sharkY + sharkSize > fish[i].y)) {
            // Colidiu com um peixe
            fish[i].x = tft.width() + random(20, 100);
            fish[i].y = random(10, tft.height() - 20);
            score++;
        }
    }
}

// Função para exibir a pontuação
void displayScore() {
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.printf("Score: %d", score);
}

void shark_setup() {
    // Inicializa a tela
    tft.begin();
 //   tft.fillScreen(TFT_BLACK);

    // Inicializa a posição dos peixes
    for (int i = 0; i < 5; i++) {
        fish[i].x = tft.width() + random(20, 100);
        fish[i].y = random(10, tft.height() - 20);
        fish[i].size = 8;
    }
   shark_loop(); 
}

void shark_loop() {
        for(;;) {
    // Limpa a tela
    tft.fillScreen(TFT_BLACK);

    // Move o tubarão
    moveShark();

    // Desenha o tubarão
    drawShark();

    // Move e desenha os peixes
    for (int i = 0; i < 5; i++) {
        moveFish(fish[i]);
        drawFish(fish[i]);
    }

    // Verifica colisões
    checkCollisions();

    // Exibe a pontuação
    displayScore();

    // Pequeno atraso para controlar a velocidade do loop
    delay(50);
        }

    #ifndef CARDPUTER
    if(checkPrevPress()) { // Apertar o botão power dos sticks
      tft.fillScreen(BGCOLOR);
      goto Exit;
    }
    #else
    Keyboard.update();
    if(Keyboard.isKeyPressed('`')) {
      tft.fillScreen(BGCOLOR);
      goto Exit;
    }   // apertar ESC no Cardputer
    #endif
    Exit:
    delay(300);
    Serial.println();
}