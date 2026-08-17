#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define GAME_SPEED 100
#define II_SPEED 250
#define RANDOM_BOUNCE 1

// Пины кнопок (можно менять)
#define BTN_UP 2
#define BTN_DWN 3

// Настройки игры
#define X_PLAYER_1 5
#define X_PLAYER_2 SCREEN_WIDTH - 10
#define RACKET_LEN 8
#define RACKET_WIDTH 2

// Игровые переменные
int8_t ballPos[2];
int8_t ballSpeed[2];
uint32_t ballTimer, enemyTimer;
boolean btnFlag1, btnFlag2;
int8_t racketPos1, racketPos2 = 0;
int8_t prevRacketPos1, prevRacketPos2 = 0;
byte count1, count2;
byte speedIncr = 0, iiIncr = 0;

void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DWN, INPUT_PULLUP);
  
  // Инициализация дисплея
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while(1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  randomSeed(analogRead(A0));
  newRound();
  updateDisplay();
}

void loop() {
  ballRoutine();
  buttonTick();
  enemyTick();
}

void updateDisplay() {
  display.clearDisplay();
  
  // Ракетки
  display.fillRect(X_PLAYER_1, racketPos1, RACKET_WIDTH, RACKET_LEN, WHITE);
  display.fillRect(X_PLAYER_2, racketPos2, RACKET_WIDTH, RACKET_LEN, WHITE);
  
  // Мяч
  display.fillCircle(ballPos[0], ballPos[1], 1, WHITE);
  
  // Счет
  display.setCursor(10, 0);
  display.print(count1);
  display.setCursor(SCREEN_WIDTH - 20, 0);
  display.print(count2);
  
  display.display();
}

void enemyTick() {
  if ((long)(millis() - enemyTimer) >= (II_SPEED - iiIncr)) {
    enemyTimer = millis();
    if (racketPos2 + RACKET_LEN/2 > ballPos[1]) racketPos2--;
    else racketPos2++;
    racketPos2 = constrain(racketPos2, 0, SCREEN_HEIGHT - RACKET_LEN);
    updateDisplay();
  }
}

void buttonTick() {
  if (!digitalRead(BTN_DWN) && !btnFlag1) {
    btnFlag1 = true;
    racketPos1 += 2;
    if (racketPos1 > (SCREEN_HEIGHT - RACKET_LEN)) racketPos1 = (SCREEN_HEIGHT - RACKET_LEN);
    updateDisplay();
  }
  if (digitalRead(BTN_DWN) && btnFlag1) {
    btnFlag1 = false;
  }
  if (!digitalRead(BTN_UP) && !btnFlag2) {
    btnFlag2 = true;
    racketPos1 -= 2;
    if (racketPos1 < 0) racketPos1 = 0;
    updateDisplay();
  }
  if (digitalRead(BTN_UP) && btnFlag2) {
    btnFlag2 = false;
  }
}

void ballRoutine() {
  if ((long)(millis() - ballTimer) >= (GAME_SPEED - speedIncr)) {
    ballTimer = millis();
    int8_t prevPos[2];
    for (byte i = 0; i < 2; i++) {
      prevPos[i] = ballPos[i];
      ballPos[i] += ballSpeed[i];
    }

    // Столкновение с левой ракеткой
    if (ballPos[0] <= X_PLAYER_1 + RACKET_WIDTH) {
      if (!(prevPos[1] >= racketPos1 && prevPos[1] <= (racketPos1 + RACKET_LEN))) {
        count2++;
        updateDisplay();
        delay(1000);
        newRound();
        return;
      } else {
        ballPos[0] = X_PLAYER_1 + RACKET_WIDTH;
        ballSpeed[0] = -ballSpeed[0];
        if (RANDOM_BOUNCE) ballSpeed[1] *= (random(0, 2)) ? 1 : -1;
      }
    }
    
    // Столкновение с правой ракеткой
    if (ballPos[0] >= X_PLAYER_2) {
      if (!(prevPos[1] >= racketPos2 && prevPos[1] <= (racketPos2 + RACKET_LEN))) {
        count1++;
        updateDisplay();
        delay(1000);
        newRound();
        return;
      } else {
        ballPos[0] = X_PLAYER_2 - 1;
        ballSpeed[0] = -ballSpeed[0];
        if (RANDOM_BOUNCE) ballSpeed[1] *= (random(0, 2)) ? 1 : -1;
      }
    }
    
    // Границы экрана
    if (ballPos[1] <= 0 || ballPos[1] >= SCREEN_HEIGHT - 1) {
      ballPos[1] = constrain(ballPos[1], 0, SCREEN_HEIGHT - 1);
      ballSpeed[1] = -ballSpeed[1];
    }

    updateDisplay();
  }
}

void newRound() {
  ballPos[0] = X_PLAYER_1 + RACKET_WIDTH + 1;
  racketPos1 = random(0, SCREEN_HEIGHT - RACKET_LEN);
  ballPos[1] = racketPos1 + RACKET_LEN / 2;
  ballSpeed[0] = 2;
  ballSpeed[1] = (random(0, 2)) ? 1 : -1;
  
  // Увеличение сложности
  if (count1 >= 10) speedIncr = 10;
  if (count1 >= 20) speedIncr = 25;
  if (count1 >= 30) speedIncr = 40;
  
  updateDisplay();
}