#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define JUMP_BUTTON 7

// Dino
float dinoY;
float velocity;
bool jumping = false;

const int groundY = 54;
const int dinoX = 15;

// Cactus
int cactusX;
const int cactusWidth = 7;
const int cactusHeight = 14;

// Game
int score = 0;
bool gameOver = false;
unsigned long lastFrame = 0;
int gameSpeed = 3;

void setup() {
  pinMode(JUMP_BUTTON, INPUT_PULLUP);

  // Arduino Micro I2C: SDA = D2, SCL = D3
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  display.clearDisplay();
  display.display();

  startGame();
}

void startGame() {
  dinoY = groundY - 12;
  velocity = 0;
  jumping = false;

  cactusX = 128;
  score = 0;
  gameSpeed = 3;
  gameOver = false;
}

void loop() {

  // About 60 FPS
  if (millis() - lastFrame < 16) return;
  lastFrame = millis();

  // Restart after game over
  if (gameOver) {
    if (digitalRead(JUMP_BUTTON) == LOW) {
      delay(150);
      startGame();
    }

    drawGameOver();
    return;
  }

  // Jump
  if (digitalRead(JUMP_BUTTON) == LOW && !jumping) {
    velocity = -7;
    jumping = true;
  }

  // Gravity
  velocity += 0.4;
  dinoY += velocity;

  // Land
  if (dinoY >= groundY - 12) {
    dinoY = groundY - 12;
    velocity = 0;
    jumping = false;
  }

  // Move cactus
  cactusX -= gameSpeed;

  if (cactusX < -cactusWidth) {
    cactusX = 128 + random(20, 60);
    score++;

    // Slowly increase difficulty
    if (score % 5 == 0 && gameSpeed < 7) {
      gameSpeed++;
    }
  }

  // Collision
  if (dinoX + 10 > cactusX &&
      dinoX < cactusX + cactusWidth &&
      dinoY + 12 > groundY - cactusHeight) {
    gameOver = true;
  }

  drawGame();

  score++;
}

void drawGame() {
  display.clearDisplay();

  // Ground
  display.drawLine(0, groundY, 127, groundY, SSD1306_WHITE);

  // Dino
  drawDino(dinoX, (int)dinoY);

  // Cactus
  drawCactus(cactusX, groundY - cactusHeight);

  // Score
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(90, 2);
  display.print(score / 30);

  display.display();
}

void drawDino(int x, int y) {

  // Body
  display.fillRect(x + 2, y + 5, 8, 7, SSD1306_WHITE);

  // Head
  display.fillRect(x + 5, y + 1, 7, 7, SSD1306_WHITE);

  // Snout
  display.fillRect(x + 11, y + 4, 3, 4, SSD1306_WHITE);

  // Eye
  display.drawPixel(x + 9, y + 2, SSD1306_BLACK);

  // Legs
  display.drawLine(x + 4, y + 11, x + 4, y + 14, SSD1306_WHITE);
  display.drawLine(x + 9, y + 11, x + 9, y + 14, SSD1306_WHITE);

  // Tail
  display.drawLine(x + 2, y + 7, x - 2, y + 10, SSD1306_WHITE);
}

void drawCactus(int x, int y) {

  display.fillRect(x + 2, y, 4, cactusHeight, SSD1306_WHITE);

  // Left arm
  if (x > -10) {
    display.fillRect(x, y + 5, 3, 6, SSD1306_WHITE);
    display.fillRect(x - 2, y + 5, 3, 3, SSD1306_WHITE);
  }

  // Right arm
  display.fillRect(x + 5, y + 8, 4, 3, SSD1306_WHITE);
  display.fillRect(x + 7, y + 5, 3, 6, SSD1306_WHITE);
}

void drawGameOver() {

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(22, 18);
  display.println("GAME");

  display.setCursor(22, 36);
  display.println("OVER");

  display.setTextSize(1);
  display.setCursor(25, 55);
  display.print("Press JUMP");

  display.display();
}
