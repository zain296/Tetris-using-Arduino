#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX_DEVICES 4 

#define CS_PIN     10
#define BUZZER_PIN 8


#define LEFT_BTN    3
#define RIGHT_BTN   4
#define ROTATE_BTN  5
#define DROP_BTN    6

#define WIDTH 8
#define HEIGHT 32

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


byte gameBoard[HEIGHT][WIDTH] = {0}; 

int curX = 3, curY = 0; 
uint8_t currentShape[4];
uint8_t nextShape[4];

const uint8_t shapes[7][4] = {
  {0b0100,0b0100,0b0100,0b0100}, // I
  {0b0110,0b0110,0b0000,0b0000}, // O
  {0b0100,0b1110,0b0000,0b0000}, // T
  {0b0110,0b1100,0b0000,0b0000}, // S
  {0b1100,0b0110,0b0000,0b0000}, // Z
  {0b1000,0b1110,0b0000,0b0000}, // J
  {0b0010,0b1110,0b0000,0b0000}  // L
};

unsigned long lastTick = 0, lastMove = 0, lastRotate = 0, lastFrame = 0;
int tickSpeed = 400;
bool gameOver = false;

void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 1);
  mx.clear();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LEFT_BTN, INPUT_PULLUP);
  pinMode(RIGHT_BTN, INPUT_PULLUP);
  pinMode(ROTATE_BTN, INPUT_PULLUP);
  pinMode(DROP_BTN, INPUT_PULLUP);

  randomSeed(analogRead(A5));
  pickNewPiece(nextShape);
  spawnPiece();
}

void resetGame() {
  for(int y=0; y<HEIGHT; y++) {
    for(int x=0; x<WIDTH; x++) {
      gameBoard[y][x] = 0;}
  }
  tickSpeed = 400;
  gameOver = false;
  pickNewPiece(nextShape);
  spawnPiece();
  mx.clear();
}

void pickNewPiece(uint8_t* shapePtr) {
  int r = random(0,7);
  for(int i=0;i<4;i++) {
    shapePtr[i] = shapes[r][i];
    }
}

void spawnPiece() {
  memcpy(currentShape, nextShape, 4);
  pickNewPiece(nextShape);
  curX = 2;
  curY = 0;

  if (checkCollision(curX, curY)) {
    gameOver = true;
    tone(BUZZER_PIN, 400, 200);
    delay(200);
    tone(BUZZER_PIN, 200, 400);
  }
}

bool checkCollision(int nx, int ny) {
  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      if(bitRead(currentShape[i], j)){
        int tx = nx + j;
        int ty = ny + i;
        if(tx < 0 || tx >= WIDTH || ty < 0 || ty >= HEIGHT) {
          return true;
          }
        if(gameBoard[ty][tx] == 1) {
          return true; 
          }
      }
    }
  }
  return false;
}

void rotatePiece() {
  uint8_t temp[4] = {0};
  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      if(bitRead(currentShape[i], j)) {
        bitSet(temp[j], 3-i);
        }
    }
  }
  uint8_t backup[4];
  memcpy(backup, currentShape, 4);
  memcpy(currentShape, temp, 4);

  if(checkCollision(curX, curY)){
    if(!checkCollision(curX-1, curY)) {
      curX--;
      }
    else if(!checkCollision(curX+1, curY)) {
      curX++;
      }
    else {
      memcpy(currentShape, backup, 4);
      return; 
    }
  }
  tone(BUZZER_PIN, 700, 60);
}

void lockPiece() {
  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      if(bitRead(currentShape[i], j)){
        if(curY+i < HEIGHT && curX+j < WIDTH)
          gameBoard[curY+i][curX+j] = 1; 
      }
    }
  }

  if(tickSpeed > 100) tickSpeed -= 5;

  // LINE CLEAR LOGIC
  for(int y = HEIGHT-1; y >= 0; y--){
    bool fullLine = true;
    for(int x = 0; x < WIDTH; x++){
      if(gameBoard[y][x] == 0) { 
        fullLine = false;
         break;
         }
    }

    if(fullLine){
      tone(BUZZER_PIN, 1000, 120);
      for(int k = y; k > 0; k--){
        for(int x = 0; x < WIDTH; x++) gameBoard[k][x] = gameBoard[k-1][x];
      }
      for(int x = 0; x < WIDTH; x++) gameBoard[0][x] = 0;
      y++; 
    }
  }
  spawnPiece();
}

void loop() {
  unsigned long now = millis();

  if(gameOver){
    mx.clear();
    mx.update();
    if(digitalRead(DROP_BTN) == LOW){
      delay(300);
      resetGame();
    }
    return;
  }

  // INPUT HANDLING
  if(now - lastMove > 120){
    if(digitalRead(LEFT_BTN) == LOW && !checkCollision(curX-1, curY)){
      curX--; lastMove = now;
    }
    else if(digitalRead(RIGHT_BTN) == LOW && !checkCollision(curX+1, curY)){
      curX++; lastMove = now;
    }
  }

  if(digitalRead(ROTATE_BTN) == LOW && (now - lastRotate > 200)){
    rotatePiece();
    lastRotate = now;
  }

  int speed = (digitalRead(DROP_BTN) == LOW) ? 60 : tickSpeed;

  if(now - lastTick > speed){
    if(!checkCollision(curX, curY+1)) curY++;
    else lockPiece();
    lastTick = now;
  }

  if(now - lastFrame < 30) return;
  lastFrame = now;

  mx.clear();
  for(int y=0; y<HEIGHT; y++){
    for(int x=0; x<WIDTH; x++){
      if(gameBoard[y][x] == 1) mx.setPoint(x, y, true);
    }
  }

  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      if(bitRead(currentShape[i], j)) mx.setPoint(curX+j, curY+i, true);
    }
  }
  mx.update();
}