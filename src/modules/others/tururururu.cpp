#include "core/mykeyboard.h"
#include "core/globals.h"
#include "core/display.h"
#include "tururururu.h"

//By: @IncursioHack / github.com/IncursioHack

// Configuração do personagem principal (tubarão)
int sharkX = 40;
int sharkY = 80;
int sharkSize = 14;

//Configuração dos peixes
struct Fish {
    int x, y, size;
};
Fish fish[5];  // Array de peixes

// Configuração de pontuação
int score = 0;

void initSprites() {
    //sprite para desenhar a tela toda
    //tft.deleteSprite();
    //tft.createSprite(WIDTH,HEIGHT);
    tft.fillScreen(bruceConfig.bgColor);

    //menu_op para desenhar o tubarao
    sprite.deleteSprite();
    sprite.createSprite(32,26);
    sprite.fillScreen(bruceConfig.bgColor);
    sprite.fillEllipse(19,16,10,5,TFT_DARKGREY);
    sprite.fillCircle(17,23,5,TFT_LIGHTGREY);
    sprite.fillTriangle(0,9,0,21,9,16, TFT_DARKGREY);
    sprite.fillTriangle(17,5,17,13,22,13, TFT_DARKGREY);
    sprite.fillCircle(25,13,1,TFT_RED);
    sprite.fillTriangle(23,17,29,17,24,20,TFT_RED);
    sprite.fillRect(0,21,32,5,bruceConfig.bgColor);

    //draw para desenhar o peixe
    draw.deleteSprite();
    draw.createSprite(20,8);
    draw.fillScreen(bruceConfig.bgColor);
    draw.fillEllipse(6,4, 6, 3, TFT_ORANGE);
    draw.fillTriangle(16,0,16,8,11,5,TFT_ORANGE);
    draw.drawFastVLine(6,1,7,TFT_WHITE);
    draw.drawFastVLine(10,1,7,TFT_WHITE);
    draw.drawFastVLine(15,1,7,TFT_WHITE);
    draw.fillCircle(3,3,1,TFT_BLACK);

}

// Função para desenhar o tubarão
void drawShark() {
    sprite.pushSprite(sharkX-sharkSize, sharkY-7);
    //sprite.pushToSprite(&sprite,sharkX-sharkSize, sharkY,TFT_TRANSPARENT);
}

// Função para desenhar peixes
void drawFish(Fish &f) {
    draw.pushSprite(f.x,f.y);
    //draw.pushToSprite(&sprite,f.x,f.y,TFT_TRANSPARENT);
}

// Função para mover o tubarão
void moveShark() {

    #if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2) || defined(M5STACK) // checkEscPress is the same of checkPrevPress in these devices
    if (checkSelPress())
    #else
    if (checkPrevPress())
    #endif
    {
        sharkY -= 2;  // Move para cima
    }

    if (checkNextPress())
    {
        sharkY += 2;  // Move para baixo
    }
    if (sharkY < 0) {
        sharkY = 0;
    }
    if (sharkY > HEIGHT - sharkSize) {
        sharkY = HEIGHT - sharkSize;
    }
}

// Função para mover peixes
void moveFish(Fish &f) {
    f.x -= 2;  // Move o peixe para a esquerda
    if (f.x < -10) {
        tft.fillRect(f.x,f.y,22,11,bruceConfig.bgColor);
        f.x = WIDTH + random(20, 100);
        f.y = random(10, HEIGHT - 20);
    }
}

// Função para verificar colisões entre o tubarão e os peixes
void checkCollisions() {
    for (int i = 0; i < 5; i++) {
        if ((sharkX < fish[i].x + fish[i].size) && (sharkX + sharkSize > fish[i].x) &&
            (sharkY < fish[i].y + fish[i].size) && (sharkY + sharkSize > fish[i].y)) {
            // Colidiu com um peixe
            tft.fillRect(fish[i].x,fish[i].y,18,8,bruceConfig.bgColor);
            fish[i].x = WIDTH + random(20, 100);
            fish[i].y = random(10, HEIGHT - 20);
            score++;
        }
    }
}

// Função para exibir a pontuação
void displayScore() {
    tft.setTextColor(TFT_WHITE,bruceConfig.bgColor);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.printf("Score: %d", score);
}

void shark_setup() {
    // Inicializa a posição dos peixes
    for (int i = 0; i < 5; i++) {
        fish[i].x = WIDTH + random(20, 100);
        fish[i].y = random(10, HEIGHT - 20);
        fish[i].size = 8;
    }
    //desenha peixes e inicia o display
    initSprites();
    //inicia o jogo
    shark_loop();

}

void shark_loop() {
    MegaFooter();
    for(;;) {
        // Mostra a tela

        // Começa a desenhar o Sprite
        displayScore();
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

        // Pequeno atraso para controlar a velocidade do loop
        if(score<10) delay(50);
        if(score>=10 && score<20) delay(40);
        if(score>=20 && score<30) delay(35);
        if(score>=30 && score<40) delay(30);
        if(score>=40 && score<50) delay(25);
        if(score>=50) { delay(15); }

        if(score==99) {
            displaySuccess("So...");
            while(!checkSelPress()) { yield(); }
            while(checkSelPress()) { yield(); } //debounce
            displaySuccess("you just found");
            while(!checkSelPress()) { yield(); }
            while(checkSelPress()) { yield(); } //debounce
            displayInfo("hidden credits!");
            while(!checkSelPress()) { yield(); }
            while(checkSelPress()) { yield(); } //debounce
            displayInfo("main devs:");
            while(!checkSelPress()) { yield(); }
            while(checkSelPress()) { yield(); } //debounce
            options = {
                {"bmorcelli", [=]() { displayRedStripe("github.com/bmorcelli");delay(2000); }},
                {"pr3y", [=]() { displayRedStripe("github.com/pr3y");delay(2000); }},
                {"IncursioHack", [=]() { displayRedStripe("github.com/IncursioHack");delay(2000); }},
            };

            delay(200);
            loopOptions(options);
            delay(200);

            initSprites();
            score++;
        }

        if(checkEscPress()) {
            returnToMenu=true;
            goto Exit;
        }
    }

    Exit:
    delay(150);
    Serial.println();
}
