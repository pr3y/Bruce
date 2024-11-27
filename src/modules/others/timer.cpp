/**
 * @file timer.cpp
 * @author Aleksei Gor (https://github.com/AlekseiGor)
 * @brief Timer
 * @version 0.1
 * @date 2024-11-26
 */

#include "core/display.h"

// Размеры экрана и начальные координаты
int iconX = WIDTH / 2 - 40;
int iconY = 27 + (HEIGHT - 134) / 2;

void timerLoop() {
    unsigned long startMillis = millis(); // Запоминаем время старта
    unsigned long duration = 10000;       // Длительность таймера в миллисекундах (10 секунд)
    unsigned long currentMillis;
    unsigned long elapsedMillis;

    char timeString[9]; // Для отображения строки формата "HH:MM:SS"

    tft.fillScreen(bruceConfig.bgColor);
    delay(300);

    // Логируем старт таймера
    Serial.println("Timer started");

    for (;;) {
        currentMillis = millis(); // Текущее время
        elapsedMillis = currentMillis - startMillis;

        // Лог текущего времени и прошедшего времени
        Serial.print("Current millis: ");
        Serial.println(currentMillis);
        Serial.print("Elapsed millis: ");
        Serial.println(elapsedMillis);

        // Если таймер истёк
        if (elapsedMillis >= duration) {
            Serial.println("Timer expired");
            tft.fillScreen(bruceConfig.bgColor);
            tone(500, 500);
            returnToMenu = true;
            break;
        }

        // Вычисляем оставшееся время
        unsigned long remainingMillis = duration - elapsedMillis;
        int seconds = (remainingMillis / 1000) % 60;
        int minutes = (remainingMillis / 60000) % 60;
        int hours = (remainingMillis / 3600000);

        // Форматируем строку для отображения
        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", hours, minutes, seconds);

        // Лог оставшегося времени
        Serial.print("Remaining time: ");
        Serial.println(timeString);

        // Рисуем рамку и отображаем таймер
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawRect(10, 10, WIDTH - 15, HEIGHT - 15, bruceConfig.priColor);
        tft.setCursor(64, HEIGHT / 3 + 5);
        tft.setTextSize(4);
        tft.drawCentreString(timeString, WIDTH / 2, HEIGHT / 2 - 13, 1);

        // Проверка на нажатие кнопок для выхода
        if (checkSelPress() || checkEscPress()) {
            Serial.println("Button pressed: Exiting timer");
            tft.fillScreen(bruceConfig.bgColor);
            returnToMenu = true;
            break;
        }

        delay(100); // Обновляем экран каждые 100 мс
    }
}

void timerSetup() {
    // Инициализация последовательного порта
    Serial.begin(115200);
    while (!Serial) {
        ; // Ждём, пока последовательный порт будет готов
    }
    Serial.println("Timer setup initialized");
    timerLoop();
}
