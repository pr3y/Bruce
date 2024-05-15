#include "mykeyboard.h"
#include "tururururu.h"
#include "globals.h"
#include "display.h"

//By: @IncursioHack / github.com/IncursioHack

// Configuração do personagem principal (tubarão)
int sharkX = 40;
int sharkY = 80;
int sharkSize = 16;

//Configuração dos peixes
struct Fish {
    int x, y, size;
};
Fish fish[5];  // Array de peixes

// Configuração de pontuação
int score = 0;

void initSprites() {
    //sprite para desenhar a tela toda
    sprite.deleteSprite();
    sprite.createSprite(WIDTH,HEIGHT);

    //menu_op para desenhar o tubarao
    menu_op.deleteSprite();
    menu_op.createSprite(32,16);
    menu_op.fillScreen(TFT_TRANSPARENT);
    menu_op.fillEllipse(19,11,10,5,TFT_DARKGREY);
    menu_op.fillCircle(17,18,5,TFT_LIGHTGREY);
    menu_op.fillTriangle(0,4,0,16,9,11, TFT_DARKGREY);
    menu_op.fillTriangle(17,0,17,8,22,8, TFT_DARKGREY);
    menu_op.fillCircle(25,8,1,TFT_RED);
    menu_op.fillTriangle(23,12,29,12,24,15,TFT_RED);

    //draw para desenhar o peixe
    draw.deleteSprite();
    draw.createSprite(16,8);
    draw.fillScreen(TFT_TRANSPARENT);
    draw.fillEllipse(6,4, 6, 3, TFT_ORANGE);
    draw.fillTriangle(16,0,16,8,11,5,TFT_ORANGE);
    draw.drawFastVLine(6,1,7,TFT_WHITE);
    draw.drawFastVLine(10,1,7,TFT_WHITE);
    draw.drawFastVLine(15,1,7,TFT_WHITE);
    draw.fillCircle(3,3,1,TFT_BLACK);

}

// Função para desenhar o tubarão
void drawShark() {
    menu_op.pushToSprite(&sprite,sharkX-sharkSize, sharkY,TFT_TRANSPARENT);
}

// Função para desenhar peixes
void drawFish(Fish &f) {
    draw.pushToSprite(&sprite,f.x,f.y,TFT_TRANSPARENT);
}

// Função para mover o tubarão
void moveShark() {

    #ifndef CARDPUTER
    if (checkSelPress()) 
    #else
    Keyboard.update();
    if (Keyboard.isKeyPressed(';'))
    #endif
    {
        sharkY -= 2;  // Move para cima
    }
    #ifndef CARDPUTER
    if (checkNextPress()) 
    #else
    Keyboard.update();
    if (Keyboard.isKeyPressed('.'))
    #endif
    {
        sharkY += 2;  // Move para baixo
    }
    if (sharkY < 0) {
        sharkY = 0;
    }
    if (sharkY > sprite.height() - sharkSize) {
        sharkY = sprite.height() - sharkSize;
    }
}

// Função para mover peixes
void moveFish(Fish &f) {
    f.x -= 2;  // Move o peixe para a esquerda
    if (f.x < -10) {
        f.x = sprite.width() + random(20, 100);
        f.y = random(10, sprite.height() - 20);
    }
}

// Função para verificar colisões entre o tubarão e os peixes
void checkCollisions() {
    for (int i = 0; i < 5; i++) {
        if ((sharkX < fish[i].x + fish[i].size) && (sharkX + sharkSize > fish[i].x) &&
            (sharkY < fish[i].y + fish[i].size) && (sharkY + sharkSize > fish[i].y)) {
            // Colidiu com um peixe
            fish[i].x = sprite.width() + random(20, 100);
            fish[i].y = random(10, sprite.height() - 20);
            score++;
        }
    }
}

// Função para exibir a pontuação
void displayScore() {
    sprite.fillRect(0,0, WIDTH, HEIGHT, TFT_BLACK);
    sprite.setTextColor(TFT_WHITE);
    sprite.setTextSize(2);
    sprite.setCursor(0, 0);
    sprite.printf("Score: %d", score);
}

void shark_setup() {
    // Inicializa a posição dos peixes
    for (int i = 0; i < 5; i++) {
        fish[i].x = sprite.width() + random(20, 100);
        fish[i].y = random(10, sprite.height() - 20);
        fish[i].size = 8;
    }
    //desenha peixes e inicia o display
    initSprites();
    //inicia o jogo
    shark_loop(); 

}

void shark_loop() {
    for(;;) {
        // Mostra a tela
        sprite.pushSprite(0,0);

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
        if(score>=20 && score<30) delay(30);
        if(score>=30 && score<40) delay(20);
        if(score>=40 && score<50) delay(10);
        if(score>=50) { yield(); }
        if(score==99) {
            displaySuccess("Is this fun??");
            while(!checkSelPress()) { yield(); }
            while(checkSelPress()) { yield(); } //debounce
            displaySuccess("there is more..");
            while(!checkSelPress()) { yield(); }
            while(checkSelPress()) { yield(); } //debounce
            displayInfo("in the /Oc34N");
            while(!checkSelPress()) { yield(); }
            while(checkSelPress()) { yield(); } //debounce
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