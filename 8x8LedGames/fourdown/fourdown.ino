#include "LedControl.h"
/*
 * pin 5 left button
 * pin 3 right button
 */
const int buttonLeft = 5;
const int buttonRight = 3;
/*
 pin 12 is connected to the DataIn 
 pin 11 is connected to LOAD(CS)
 pin 10 is connected to the CLK 
 */
const int dataIn =12;
const int cs=11;
const int clk=10;
LedControl lc=LedControl(dataIn,clk,cs,1);

/**
 * Digits from 0-9 to display points
 */
 const byte digits[10][5]={
  {B01110000,B01010000,B01010000,B01010000,B01110000}, //Zero
  {B00100000,B00100000,B00100000,B00100000,B00100000}, //One
  {B01110000,B00010000,B01110000,B01000000,B01110000}, //Two
  {B01110000,B00010000,B01110000,B00010000,B01110000}, //Three
  {B01010000,B01010000,B01110000,B00010000,B00010000}, //Four
  {B01110000,B01000000,B01110000,B00010000,B01110000}, //Five
  {B01110000,B01000000,B01110000,B01010000,B01110000}, //Six
  {B01110000,B00010000,B00010000,B00010000,B00010000}, //Seven
  {B01110000,B01010000,B01110000,B01010000,B01110000}, //Eight
  {B01110000,B01010000,B01110000,B00010000,B01110000}  //Nine
  };

/**
 * The position of the player on row 7
 */
byte pos=B00011000;

/**
 * The row of the current block obstacle
 */
byte block_pos=0;

/**
 * The vertical position of the block obstacle in two bytes
 * b2 is the lower one to test collision with
 */
byte b1=B00011000;
byte b2=B00011000;

/**
 * points are increased with each new generated block obstacle
 */
int points=95;

/**
 * counting rounds. Usually the player is allowed to move each
 * round, but the obstacle is not - depending on speed var.
 */
int gameRound = 0;

/**
 * As higher the speed, as slower the game
 * if speed is 6 the obstacle is moved only every sixth gameround
 */
int speed=6;

int highestScore=99;

void setup() {
  initSystem();
  startPosition();
  block_pos=0;
  gameRound=0;
  points=95;
  printDigit(points);
  delay(2000);
}

void initSystem(){
  lc.shutdown(0,false);
  lc.setIntensity(0,3);
  lc.clearDisplay(0);
  pinMode(buttonLeft,INPUT_PULLUP);
  pinMode(buttonRight,INPUT_PULLUP);
  Serial.begin(9600);
  randomSeed(analogRead(0));
}


void startPosition(){
   points=0;
   b1=B00011000;
   b2=B00011000;
   pos=B00011000;
   block_pos=0;
   gameRound=0;
   speed=6;
   lc.setRow(0,7,pos);
   delay(10);
   lc.clearDisplay(0);
   delay(10);
   lc.setRow(0,7,pos);
}

void loop() {
  
  if(gameRound % speed == 0){
    newBlock();
    moveBlock();
    if(collision()){
      youLooseTryAgain();
    }
  }
  readPlayerInput();
  delay(120);
  gameRound++;
  if(gameRound%70==0){
    speed--;
    if(speed == 0){
      speed=3;
    }
  }
}

void newBlock(){
  if(block_pos == 7){
    lc.setRow(0,7,B00000000| pos);
    block_pos=0; 
    delay(20); 
    points++;
    if(points == highestScore){
     youWonTryAgain();
    }
    generateBlock();
  }
}

void moveBlock(){
   byte empty=B00000000;
   if(block_pos-1 !=8){
    lc.setRow(0,block_pos-1,empty);
   }
   if (block_pos != 8){
     lc.setRow(0,block_pos,empty);
   }
  lc.setRow(0,block_pos,b1);
  lc.setRow(0,block_pos+1,b2);
  block_pos++;
  if(block_pos ==7){
    lc.setRow(0,6,empty);
    lc.setRow(0,7, b1 | pos);
  }
  
}

void youWonTryAgain(){
    runWinnerAnimation();
    delay(20);
    runWinnerAnimation();
    printDigit(points); 
    delay(400);  
    waitForRestart();
}

void youLooseTryAgain(){
    runLooserAnimation();
    delay(20);
    runLooserAnimation();
    printDigit(points); 
    delay(400);  
    waitForRestart();
}

void waitForRestart(){
    while(true){
       int buttonLeftState = digitalRead(buttonLeft);
       int buttonRightState = digitalRead(buttonRight);
       if(buttonLeftState == LOW && buttonRightState == LOW){
        break;
       }
    }
    startPosition();
}

boolean collision(){
  if(block_pos !=7){
    return false;
  }
  return  pos & b2;
}

void generateBlock(){
 int randNumber = random(0,7);
  b1=B0000011 << randNumber;
  b2=B0000011 << randNumber;
}

void printDigit(int num){
  if(num > 99 || num <0){
    num=0;
  }
   for(int i=0;i<5;i++){
     lc.setRow(0,i,digits[num/10][i] <<  1 | digits[num%10][i] >>  4 );
   }
}

void readPlayerInput(){
  int buttonLeftState = digitalRead(buttonLeft);
  goLeft(buttonLeftState);
  int buttonRightState = digitalRead(buttonRight);
  goRight(buttonRightState);
  lc.setRow(0,7,pos);
}

void goLeft(int state){
  if(state == LOW){
   if(pos <= 96){
    pos=pos<<1;
   }
  }
}

void goRight(int state){
  if(state == LOW){
    if(pos > 3){
     pos=pos>>1;
    }
  }
}

void runLooserAnimation(){
    byte looser[10][8]={
  {B00000000, B00000000,B00000000,B01010101,B10101010,B00000000,B00000000,B00000000},
  {B00000000, B00000000,B00000000,B10101010,B01010101,B00000000,B00000000,B00000000},
  };
  for(int i=0;i<8;i++){
    lc.setRow(0,i,looser[0][i]);
   }
    delay(40);
    lc.clearDisplay(0);
     for(int i=0;i<8;i++){
    lc.setRow(0,i,looser[1][i]);
     }
    delay(40);
    lc.clearDisplay(0);
      for(int i=0;i<8;i++){
    lc.setRow(0,i,looser[0][i]);
   }
    delay(40);
    lc.clearDisplay(0);
     for(int i=0;i<8;i++){
    lc.setRow(0,i,looser[1][i]);
     }
    delay(40);
    lc.clearDisplay(0);
}
void runWinnerAnimation(){
   byte winner[10][8]={
  {B00000000, B01100110,B11111111,B01111110,B00111100,B00011000,B00000000,B00000000},
  {B11011000,B11111000,B01110000,B00100000,B00011011,B00011111,B00001110,B00000100}
  };
  for(int i=0;i<8;i++){
    lc.setRow(0,i,winner[0][i]);
   }
    delay(2000);
    lc.clearDisplay(0);
     for(int i=0;i<8;i++){
    lc.setRow(0,i,winner[1][i]);
     }
    delay(2000);
    lc.clearDisplay(0);
}

void testPrint(){
 for(int i=0;i<100;i++){
  printDigit(i);
  delay(2000);
   lc.clearDisplay(0);
 }
}

