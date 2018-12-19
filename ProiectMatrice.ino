#include "LedControl.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

//defining the controls of the game

#define JOY_X A0
#define JOY_Y A1
#define JOY_BTN 2
#define MENU_BTN 13
#define V0_PIN 9 

LiquidCrystal lcd(8, 3, 4, 5, 6, 7);


bool currentPiece[16][4];
int currentPieceY;
int currentPieceX;
int currentPieceRotation;
int oldPieceX = 0;
int oldWantToTurn = 0;
long lastMove = 0;
const long moveDelay = 100;
const long menuMoveDelay = 150;
long dropDelay = 700;
int oldButton = 0;
int prevX = 3, prevY = 3, posX = 3, posY = 3;
int buttonState = 0;
int valX, valY, line = 0;
bool movedX = false, movedY = false;
int rotation = 0;
long lastDrop;
int score = 0;
int menuItem = 0;
int address = 0;
LedControl lc = LedControl(12, 11, 10, 1); //DIN, CLK, LOAD, No. DRIVER


// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219
  bool matrix[8][8] = {
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0}
};

const bool pieceI[16][4] = {

  {0, 1, 0, 0},
  {0, 1, 0, 0}, 
  {0, 1, 0, 0}, 
  {0, 1, 0, 0},

  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {1, 1, 1, 1},

  {0, 1, 0, 0},
  {0, 1, 0, 0}, 
  {0, 1, 0, 0}, 
  {0, 1, 0, 0},

  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {1, 1, 1, 1}
  
};

const bool pieceT[16][4] = {

  {0, 1, 0, 0},
  {0, 1, 1, 0}, 
  {0, 1, 0, 0}, 
  {0, 0, 0, 0},

  {0, 0, 0, 0},
  {1, 1, 1, 0},
  {0, 1, 0, 0},
  {0, 0, 0, 0},

  {0, 1, 0, 0},
  {1, 1, 0, 0}, 
  {0, 1, 0, 0}, 
  {0, 0, 0, 0},

  {0, 1, 0, 0},
  {1, 1, 1, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
  
};

const bool pieceZ1[16][4] = {

  {1, 1, 0, 0},
  {0, 1, 1, 0}, 
  {0, 0, 0, 0}, 
  {0, 0, 0, 0},

  {0, 1, 0, 0},
  {1, 1, 0, 0},
  {1, 0, 0, 0},
  {0, 0, 0, 0},

  {1, 1, 0, 0},
  {0, 1, 1, 0}, 
  {0, 0, 0, 0}, 
  {0, 0, 0, 0},

  {0, 1, 0, 0},
  {1, 1, 0, 0},
  {1, 0, 0, 0},
  {0, 0, 0, 0}
  
};

const bool pieceZ2[16][4] = {

  {0, 1, 1, 0},
  {1, 1, 0, 0}, 
  {0, 0, 0, 0}, 
  {0, 0, 0, 0},

  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 0},

  {0, 1, 1, 0},
  {1, 1, 0, 0}, 
  {0, 0, 0, 0}, 
  {0, 0, 0, 0},

  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 0}
  
};

const bool pieceSquare[16][4] = {

  {1, 1, 0, 0},
  {1, 1, 0, 0}, 
  {0, 0, 0, 0}, 
  {0, 0, 0, 0},

  {1, 1, 0, 0},
  {1, 1, 0, 0}, 
  {0, 0, 0, 0}, 
  {0, 0, 0, 0},
  
  {1, 1, 0, 0},
  {1, 1, 0, 0}, 
  {0, 0, 0, 0}, 
  {0, 0, 0, 0},

  {1, 1, 0, 0},
  {1, 1, 0, 0}, 
  {0, 0, 0, 0}, 
  {0, 0, 0, 0}
  
};

const bool pieceL1[16][4] = {

  {0, 1, 0, 0},
  {0, 1, 0, 0}, 
  {0, 1, 1, 0}, 
  {0, 0, 0, 0},

  {0, 0, 0, 0},
  {1, 1, 1, 0}, 
  {1, 0, 0, 0}, 
  {0, 0, 0, 0},
  
  {1, 1, 0, 0},
  {0, 1, 0, 0}, 
  {0, 1, 0, 0}, 
  {0, 0, 0, 0},

  {0, 0, 1, 0},
  {1, 1, 1, 0}, 
  {0, 0, 0, 0}, 
  {0, 0, 0, 0}
  
};

const bool pieceL2[16][4] = {

  {0, 1, 0, 0},
  {0, 1, 0, 0}, 
  {1, 1, 0, 0}, 
  {0, 0, 0, 0},

  {1, 0, 0, 0},
  {1, 1, 1, 0}, 
  {0, 0, 0, 0}, 
  {0, 0, 0, 0},
  
  {0, 1, 1, 0},
  {0, 1, 0, 0}, 
  {0, 1, 0, 0}, 
  {0, 0, 0, 0},

  {0, 0, 0, 0},
  {1, 1, 1, 0}, 
  {0, 0, 1, 0}, 
  {0, 0, 0, 0}
  
};
void EEPROMWriteInt(int value) {
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);

  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}

int EEPROMReadInt() {
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);

  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
}

void highScoreReplace() {
  int highScore = EEPROMReadInt();
  if (highScore < score)
    EEPROMWriteInt(score);
}

void resetGame() {
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      matrix[i][j] = 0;
  score = 0;
  lcd.clear();
  lcd.setCursor(2,7);
  lcd.print(score);
  chooseNewPiece();
  return;
}

void menu() {
  int select = 0;
  long int lastMenuMove = lastMove;
  while (1) {

    if (millis() - lastMenuMove > menuMoveDelay) {
      lastMenuMove = millis();
      int valY = analogRead(JOY_Y);
      if (valY > 600)
        menuItem++;
      if (valY < 400)
        menuItem--;
      if (menuItem > 2)
        menuItem = 0;
      if (menuItem < 0)
        menuItem = 2;
      Serial.println(menuItem);
      switch (menuItem) {
      case 0:
        lcd.clear();
        lcd.print("Reset Game");
        select = digitalRead(JOY_BTN);
        if (select == 1) {
          resetGame();
          return;
        }
        break;
      case 1:
        lcd.clear();
        lcd.print("High score");
        select = digitalRead(JOY_BTN);
        if (select == 1) {
          lcd.clear();
          lcd.print(EEPROMReadInt());
        }
        break;
      case 2:
        lcd.clear();
        lcd.print("Return");
        select = digitalRead(JOY_BTN);
        if (select == 1)
          {
          lcd.clear();
          lcd.setCursor(2,7);
          lcd.print(score);
          return;
          }
        break;
      default:
        break;
      }
    }

  }

}

void startMenu() {
  int select = 0;
  long int lastMenuMove = lastMove;
  while (1) {

    if (millis() - lastMenuMove > menuMoveDelay) {
      lastMenuMove = millis();
      int valY = analogRead(JOY_Y);
      if (valY > 600)
        menuItem++;
      if (valY < 400)
        menuItem--;
      if (menuItem > 1)
        menuItem = 0;
      if (menuItem < 0)
        menuItem = 1;
      Serial.println(menuItem);
      switch (menuItem) {
      case 0:
        lcd.clear();
        lcd.print("New Game");
        select = digitalRead(JOY_BTN);
        if (select == 1) {
          resetGame();
          return;
        }
        break;
      case 1:
        lcd.clear();
        lcd.print("High score");
        select = digitalRead(JOY_BTN);
        if (select == 1) {
          lcd.clear();
          lcd.print(EEPROMReadInt());
        }
        break;
      default:
        break;
      }
    }

  }

}

//We choose a new piece by randomising a number from 0 to 6 to get the index of the next piece that we will get

void chooseNewPiece() {
 
  int i, j, k;
  j = random() % 7;
  switch (j) {
  case 0:
    for (i = 0; i < 16; i++)
      for (k = 0; k < 4; k++)
        currentPiece[i][k] = pieceI[i][k];
    break;
  case 1:
    for (i = 0; i < 16; i++)
      for (k = 0; k < 4; k++)
        currentPiece[i][k] = pieceT[i][k];
    break;
  case 2:
    for (i = 0; i < 16; i++)
      for (k = 0; k < 4; k++)
        currentPiece[i][k] = pieceZ1[i][k];
    break;
  case 3:
    for (i = 0; i < 16; i++)
      for (k = 0; k < 4; k++)
        currentPiece[i][k] = pieceZ2[i][k];
    break;
  case 4:
    for (i = 0; i < 16; i++)
      for (k = 0; k < 4; k++)
        currentPiece[i][k] = pieceSquare[i][k];
    break;
  case 5:
    for (i = 0; i < 16; i++)
      for (k = 0; k < 4; k++)
        currentPiece[i][k] = pieceL1[i][k];
    break;
  case 6:
    for (i = 0; i < 16; i++)
      for (k = 0; k < 4; k++)
        currentPiece[i][k] = pieceL2[i][k];
    break;
  }

  currentPieceX = 3;
  currentPieceY = -4;
  currentPieceRotation = 0;

}

/* We scan the matrix for rows that are full in the "removeRows" function then we call "deleteRows" sending 
   it the row index then we copy the rows above one row down and we make the top row empty*/

void deleteRow(int y) {
  while (y > 0) {
    for (int x = 0; x < 8; ++x) {
      matrix[y][x] = matrix[y - 1][x];
    }
    --y;
  }
  for (int x = 0; x < 8; ++x) {
    matrix[0][x] = 0;
  }
  score += 50;
  if( score > 200) dropDelay = 650;
  if( score > 300) dropDelay = 600;
  if( score > 400) dropDelay = 550;
  if( score > 500) dropDelay = 500;
  lcd.clear();
  lcd.setCursor(2, 7);
  lcd.print(score);
}

void removeRows() {
  int counter;
  for (int y = 0; y < 8; ++y) {
    counter = 0;
    for (int x = 0; x < 8; ++x) {
      if (matrix[y][x] == true)
        counter++;
    }
    if (counter == 8)
      deleteRow(y);
  }
}

int pieceOffEdge(int pieceX, int pieceY, int pieceRotation) {
  int k = 0;
  bool newPiece[4][4];
  for (int i = pieceRotation * 4; i < pieceRotation * 4 + 4; ++i) {
    for (int j = 0; j < 4; ++j)
      newPiece[k][j] = currentPiece[i][j];
    k++;
  }
  for (int i = 0; i < 4; ++i) {
    for (int x = 0; x < 4; ++x) {
      int newY = pieceY + i;
      int newX = pieceX + x;
      if (newY < 0) continue;
      if (newPiece[i][x] > 0) {
        if (newX >= 8) return 1;
        if (newX < 0) return 1;
      }
    }
  }
  return 0;
}

int pieceHitsBottom(int pieceX, int pieceY, int pieceRotation) {
  int k = 0;
  bool newPiece[4][4];
  for (int i = pieceRotation * 4; i < pieceRotation * 4 + 4; ++i) {
    for (int j = 0; j < 4; ++j)
      newPiece[k][j] = currentPiece[i][j];
    k++;
  }
  for (int y = 0; y < 4; ++y) {
    int ny = pieceY + y;
    if (ny < 0) continue;
    for (int x = 0; x < 4; ++x) {
      int nx = pieceX + x;
      if (newPiece[y][x] > 0) {
        if (ny >= 8) return 1; // yes: goes off bottom of grid      
        if (matrix[ny][nx] == 1) return 1; // yes: grid already full in this space
      }
    }
  }
  return 0; // doesn't hit
}

int pieceCanFit(int pieceX, int pieceY, int pieceRotation) {
  if (pieceOffEdge(pieceX, pieceY, pieceRotation))
    return 0;
  if (pieceHitsBottom(pieceX, pieceY, pieceRotation))
    return 0;
  return 1;
}

void erasePiece() {
  int i, j, k = 0;
  bool newPiece[4][4] = { 0 };
  for (i = currentPieceRotation * 4; i < currentPieceRotation * 4 + 4; ++i) {
    for (j = 0; j < 4; ++j)
      newPiece[k][j] = currentPiece[i][j];
    k++;
  }
  for (i = 0; i < 4; ++i)
    for (j = 0; j < 4; ++j) {
      int nx = currentPieceX + j;
      int ny = currentPieceY + i;
      if (ny < 0 || ny > 7) continue;
      if (nx < 0 || nx > 7) continue;
      if (newPiece[i][j] == true)
        matrix[ny][nx] = false;
    }
}

void placePiece() {

  int i, j, k = 0;
  bool newPiece[4][4] = {
    0
  };
  for (i = currentPieceRotation * 4; i < currentPieceRotation * 4 + 4; ++i) {
    for (j = 0; j < 4; ++j)
      newPiece[k][j] = currentPiece[i][j];
    k++;
  }
  for (i = 0; i < 4; ++i)
    for (j = 0; j < 4; ++j) {
      int nx = currentPieceX + j;
      int ny = currentPieceY + i;
      if (ny < 0 || ny > 7) continue;
      if (nx < 0 || nx > 7) continue;
      if (newPiece[i][j] == true)
        matrix[ny][nx] = true;
    }
}

void movePiece() {
  int valX = analogRead(JOY_X);

  int newPieceX = 0;

  if (valX < 400)
    newPieceX = -1;
  if (valX > 600)
    newPieceX = 1;

  if (newPieceX != oldPieceX && pieceCanFit(currentPieceX + newPieceX, currentPieceY, currentPieceRotation) == 1) {
    currentPieceX += newPieceX;
  }
  oldPieceX = newPieceX;
}

void rotatePiece() {
  int wantToTurn = 0;
  int newButton = digitalRead(JOY_BTN);

  if (newButton > 0 && oldButton != newButton) {
    wantToTurn = 1;
  }

  oldButton = newButton;

  if (wantToTurn == 1 && wantToTurn != oldWantToTurn) {
    int newPieceRotation = (currentPieceRotation + 1) % 4;
    if (pieceCanFit(currentPieceX, currentPieceY, newPieceRotation)) {
      currentPieceRotation = newPieceRotation;
    }
  }

  oldWantToTurn = wantToTurn;

}

void tryToDropPiece() {
  erasePiece();

  if (pieceCanFit(currentPieceX, currentPieceY + 1, currentPieceRotation)) {
    currentPieceY++;
    placePiece();
  } else {
    placePiece();
    removeRows();
    if (gameIsOver() == 1) {
      gameOver();
    }

    chooseNewPiece();
  }
}

int gameIsOver() {
  int x, y, k = 0;
  bool newPiece[4][4];
  for (int i = currentPieceRotation * 4; i < currentPieceRotation * 4 + 4; ++i) {
    for (int j = 0; j < 4; ++j)
      newPiece[k][j] = currentPiece[i][j];
    k++;
  }
  for (y = 0; y < 4; ++y) {
    for (x = 0; x < 4; ++x) {
      int newY = currentPieceY + y;
      int newX = currentPieceX + x;
      if (newPiece[y][x] > 0)
        if (newY < 0) return 1;
    }
  }
  return 0;
}

void reactToPlayer() {
  int callMenu = digitalRead(MENU_BTN);
  if (callMenu > 0)
    menu();
  erasePiece();
  movePiece();
  rotatePiece();
  placePiece();
}

void gameOver() {
  highScoreReplace();
  startMenu();
}

void drawMatrix() {
  for (int y = 0; y < 8; ++y)
    for (int x = 0; x < 8; ++x)
      lc.setLed(0, y, x, matrix[y][x]);
}

void setup() {
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 15); // sets brightness (0~15 possible values)
  lc.clearDisplay(0); // clear screen
  pinMode(JOY_BTN, INPUT);
  pinMode(A2, INPUT);;
  chooseNewPiece();
  randomSeed(0);
  lastMove = millis();
  lastDrop = lastMove;
  lcd.begin(16, 2);
  //lcd.clear();
  lcd.setCursor(2, 7); 
  pinMode(V0_PIN, OUTPUT); 
  analogWrite(V0_PIN, 90); 

  startMenu();
}

void loop() {

  if (millis() - lastMove > moveDelay) {
    lastMove = millis();
    reactToPlayer();
  }

  if (millis() - lastDrop > dropDelay) {
    lastDrop = millis();
    tryToDropPiece();
  }

  drawMatrix();

}
 
 
