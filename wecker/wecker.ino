#include <SPI.h>
#include <TFT.h>
#include <Wire.h>
#include <DS3231.h>
#include "UserInput.h"
#include "ScreenSegment.h"

//TFT
#define CS   10
#define DC   9
#define RESET  8  
#define SCLK 13
#define MOSI 11

//Clock
#define SDA "sda"
#define SCL "scl"

//Rotary enc
#define ROT_CLK 2
#define ROT_DT 3
#define ROT_SW 4

//Buttons
#define A1_BTN 5
#define A2_BTN 6

//Buzzer
#define BUZZ 12

//TERMO
#define TERM 7

//Clock section
DS3231 clock;
RTCDateTime  dt;

//Screen section
TFT TFTscreen = TFT(CS, DC,RESET);
byte bgcolor[3] = {255,255,255};
byte textcolor[3] = {0,0,0};
byte cursorcolor[3] = {180,180,180};
ScreenSegment none("NONE");
ScreenSegment hour("HOUR");
ScreenSegment minute("MIN");
ScreenSegment second("SEC");
ScreenSegment alarm1Hour("A1 HOUR");
ScreenSegment alarm1Minute("A1 MIN");
ScreenSegment alarm2Hour("A2 HOUR");
ScreenSegment alarm2Minute("A2 MIN");
ScreenSegment temperatur("Temp");
ScreenSegment* allSegments[8];
UserInput input;

int RotaryPosition=0;
volatile boolean TurnDetected;  
volatile boolean rotationdirection; 
int PrevPosition;   
int StepsToTake;      

void setup() {
  Serial.begin(9600);
  TFTscreen.begin();
  TFTscreen.background(bgcolor[0], bgcolor[1], bgcolor[2]);
  clock.begin();
  //Kompilierzeit
  //clock.setDateTime(__DATE__, __TIME__);
  dt=clock.getDateTime();
  
  allSegments[0]=&hour;
  allSegments[1]=&minute;
  allSegments[2]=&second;
  allSegments[3]=&alarm1Hour;
  allSegments[4]=&alarm1Minute;
  allSegments[5]=&alarm2Hour;
  allSegments[6]=&alarm2Minute;
  allSegments[7]=&none;
  
  hour.setScreenRect(10,10,50,30);
  hour.setTextPos(10,10);
  hour.setSize(4);
  hour.setUpperLimit(24);

  minute.setScreenRect(70,10,50,30);
  minute.setTextPos(70,10);
  minute.setSize(4);
  minute.setUpperLimit(60);

  second.setScreenRect(125,25,30,30);
  second.setTextPos(125,25);   
  second.setSize(2);
  second.setUpperLimit(60);

  alarm1Hour.setScreenRect(25,85,30,20);
  alarm1Hour.setTextPos(25,85);  
  alarm1Hour.setSize(2);
  alarm1Hour.setTime(clock.getAlarm1().hour);
  alarm1Hour.setUpperLimit(24);
  
  alarm1Minute.setScreenRect(60,85,30,20);
  alarm1Minute.setTextPos(60,85);
  alarm1Minute.setSize(2);
  alarm1Minute.setTime(clock.getAlarm1().minute);
  alarm1Minute.setUpperLimit(60);
 
  alarm2Hour.setScreenRect(25,105,30,20);
  alarm2Hour.setTextPos(25,105); 
  alarm2Hour.setSize(2);
  alarm2Hour.setTime(clock.getAlarm2().hour);
  alarm2Hour.setUpperLimit(24);
  
  alarm2Minute.setScreenRect(60,105,30,20);
  alarm2Minute.setTextPos(60,105);
  alarm2Minute.setSize(2);
  alarm2Minute.setTime(clock.getAlarm2().minute);
  alarm2Minute.setUpperLimit(60);

  input.mode=SELECT;
  input.activeSegment=&none;
  input.lastUserAction=dt;
  input.setc=0;
  attachInterrupt (0,isr,FALLING);
  pinMode(ROT_SW, INPUT_PULLUP);
  printTime();
}

void loop() {
  dt = clock.getDateTime();
  printAll();
  checkForAlarm();
  checkForUserInput();
  printCursor();
  delay(100);  
}

void printAll(){
  printTime();
  printAlarm("A1 ",alarm1Hour,alarm1Minute);
  printAlarm("A2 ",alarm2Hour,alarm2Minute);
  printTemperature();
}

void printTime(){
    if(dt.hour!=hour.getRecentTime()){
      if(!(input.mode == SET && input.activeSegment == &hour)){
        hour.setTime(dt.hour);
      }
      print(hour);
    }
    printSeparator();
    if(dt.minute!=minute.getRecentTime()){
      if(!(input.mode == SET && input.activeSegment == &minute)){
      minute.setTime(dt.minute);
      }
      print(minute);
    }
     if(dt.second!=second.getRecentTime()){
      if(!(input.mode == SET && input.activeSegment == &second)){
        second.setTime(dt.second);
      }
      print(second);
     }
}

void printSeparator(){
  TFTscreen.setTextSize(hour.getSize());
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.text(":",hour.getScreenRect()[2]+2,hour.getScreenRect()[1]);
}

void print(ScreenSegment& s){
  TFTscreen.setTextSize(s.getSize());
  TFTscreen.stroke(bgcolor[0], bgcolor[1], bgcolor[2]);
  TFTscreen.text(s.getRecentTimeString().c_str(),s.getTextPos()[0],s.getTextPos()[1]);
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.text(s.getTimeAsString().c_str(),s.getTextPos()[0],s.getTextPos()[1]);
}

void printCursor(){
  TFTscreen.setTextSize(input.activeSegment->getSize());
  TFTscreen.stroke(bgcolor[0], bgcolor[1], bgcolor[2]);
  TFTscreen.text(input.activeSegment->getRecentTimeString().c_str(),input.activeSegment->getTextPos()[0],input.activeSegment->getTextPos()[1]);
  TFTscreen.fill( cursorcolor[0],  cursorcolor[1],  cursorcolor[2]);
  TFTscreen.rect(input.activeSegment->getScreenRect()[0],input.activeSegment->getScreenRect()[1],input.activeSegment->getScreenRect()[2],input.activeSegment->getScreenRect()[3]);
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.text(input.activeSegment->getTimeAsString().c_str(),input.activeSegment->getTextPos()[0],input.activeSegment->getTextPos()[1]);
}

void deleteCursor(){
  TFTscreen.setTextSize(input.recentActiveSegment->getSize());
  TFTscreen.stroke(bgcolor[0], bgcolor[1], bgcolor[2]);
  TFTscreen.text(input.recentActiveSegment->getTimeAsString().c_str(),input.recentActiveSegment->getTextPos()[0],input.recentActiveSegment->getTextPos()[1]);
  TFTscreen.fill(bgcolor[0],  bgcolor[1],  bgcolor[2]);
  TFTscreen.rect(input.recentActiveSegment->getScreenRect()[0],input.recentActiveSegment->getScreenRect()[1],input.recentActiveSegment->getScreenRect()[2],input.recentActiveSegment->getScreenRect()[3]);
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.text(input.recentActiveSegment->getTimeAsString().c_str(),input.recentActiveSegment->getTextPos()[0],input.recentActiveSegment->getTextPos()[1]);
}

void printAlarm(String str,ScreenSegment hour,ScreenSegment minute){
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.setTextSize(hour.getSize()-1);
  TFTscreen.text(str.c_str(),hour.getTextPos()[0]-15,hour.getTextPos()[1]);
  TFTscreen.setTextSize(hour.getSize());
  TFTscreen.text(hour.getTimeAsString().c_str(),hour.getTextPos()[0],hour.getTextPos()[1]);
  TFTscreen.text(":",hour.getTextPos()[0]+25,hour.getTextPos()[1]);
  TFTscreen.text(minute.getTimeAsString().c_str(),minute.getTextPos()[0],minute.getTextPos()[1]);
}

void printTemperature(){
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.setTextSize(2);
  TFTscreen.text("C 20",105,105);
}

void checkForAlarm(){
  if(clock.isAlarm1() || clock.isAlarm2()){
    beep();
  }
}

void checkForUserInput(){
  cursorMode();
  cursorPosition();
}
void isr ()  {
  delay(4);  // Verzögerung für Entprellung
  if (digitalRead(ROT_CLK))
    rotationdirection= digitalRead(ROT_DT);
  else
    rotationdirection= !digitalRead(ROT_DT);
  TurnDetected = true;
}

void cursorPosition(){
    if (TurnDetected)  {
      input.recentActiveSegment=input.activeSegment;
      PrevPosition = RotaryPosition; // Vorherige Position in Variable speichern
      if (rotationdirection) {
        RotaryPosition=RotaryPosition-1;
      } // Senkt die Position um 1
      else {
        RotaryPosition=RotaryPosition+1;
      } // Erhöht die Position um 1
      TurnDetected = false;  
      if ((PrevPosition + 1) == RotaryPosition) { 
        StepsToTake=50;
        cursormv(StepsToTake);
      }
      if ((RotaryPosition + 1) == PrevPosition) {         
        StepsToTake=-50;
        cursormv(StepsToTake);
      }
    }
}

/**
 * 1 = hour
 * 2 = min
 * 3 = sec
 * 4 = a1Hour
 * 5 = a1Min
 * 6 = a2Hour
 * 7 = a2Min
 */
void cursormv(int move){
 if(input.mode == SELECT){
   if(move < 0){
    input.cursorPos++;
    if(input.cursorPos > 7){
      input.cursorPos=0;
    }
   }
   else if(move >0){
    input.cursorPos--;
     if(input.cursorPos ==255){
      input.cursorPos=7;
    }
   }
   input.activeSegment = allSegments[input.cursorPos];
   Serial.print("SELECT: ");
   Serial.println(input.activeSegment->getName());
   deleteCursor();
   printCursor(); 
 }else if(input.mode == SET){
  if(move < 0){
    input.setc++;
    if(input.setc > input.activeSegment->getUpperLimit()){
      input.setc=0;
    }
   }
   else if(move >0){
    input.setc--;
     if(input.setc ==0){
      input.setc=input.activeSegment->getUpperLimit();
    }
   }
    Serial.print("SET: ");
    Serial.println(input.setc);
    input.activeSegment->setTime(input.setc);
 }
}

void cursorMode(){
   delay(4);
   if(digitalRead(ROT_SW) == LOW){
     if(input.mode == SET){
        saveAll();
        input.mode = SELECT;
        deleteCursor();
      }else if(input.mode == SELECT){
        input.mode = SET;
      }
     
   }
}

void saveAll(){
    clock.setDateTime(dt.year,dt.month,dt.day,hour.getTime(),minute.getTime(),second.getTime());
    TFTscreen.background(bgcolor[0], bgcolor[1], bgcolor[2]);
    hour.setTime(dt.hour);
    print(hour);
    printSeparator();
    minute.setTime(dt.minute);
    print(minute);
    second.setTime(dt.second);
    print(second);
    setAlarm1(alarm1Hour.getTime(),alarm1Minute.getTime());
    setAlarm2(alarm2Hour.getTime(),alarm2Minute.getTime());
}
void beep(){
Serial.println("bbbeeeeeeeeeeeeeeeeeeeeeeeeeeppppp");
}

void setAlarm2(uint8_t hour,uint8_t minute){
  DS3231_alarm2_t mode2 = 0b00001000;//DS3231_MATCH_H_M 
  clock.setAlarm2(1,hour,minute,mode2,true);
}

void setAlarm1(uint8_t hour,uint8_t minute){
  DS3231_alarm1_t mode1 = 0b00001000;//DS3231_MATCH_H_M 
  clock.setAlarm1(1,hour,minute,0,mode1,true);
}

