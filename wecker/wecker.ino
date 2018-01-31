
#include <SPI.h>
#include <TFT.h>
#include <Wire.h>
#include <DS3231.h>


#define CS   10
#define DC   9
#define RESET  8  
#define SCLK 13
#define MOSI 11
#define RED 64
#define GREEN 0
#define BLUE 0
#define BCK_RED 0
#define BCK_GREEN 0
#define BCK_BLUE 0

DS3231 clock;
RTCDateTime dt;
RTCDateTime recent_dt;
TFT TFTscreen = TFT(CS, DC,RESET);

void setup() {
  TFTscreen.begin();
  TFTscreen.background(BCK_RED, BCK_GREEN, BCK_BLUE);
  clock.begin();
  //Kompilierzeit
  clock.setDateTime(__DATE__, __TIME__);
  dt=clock.getDateTime();
  recent_dt.hour=dt.hour-1;
  recent_dt.minute=dt.minute-1;
  recent_dt.second=dt.second-1;
  printHour();
  printSeperator();
  printMinute();
  printSecond();
  DS3231_alarm1_t mode = 0b00001000;//DS3231_MATCH_H_M 
  clock.setAlarm1(1,6,30,0,mode,true);
   clock.setAlarm1(1,14,30,0,mode,true);
//  clock.setAlarm2();
}

void loop() {
  recent_dt=dt;
  dt = clock.getDateTime();
  printTime();
  printAlarm1();
  printAlarm2();
  printTemperature();
  delay(250);  
}

void printTime(){

  if(dt.hour!=recent_dt.hour){
    printHour();
  }
  if(dt.minute!=recent_dt.minute){
    printMinute();
  }
  printSecond();
}
void printSeperator(){
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(RED, GREEN, BLUE);
  TFTscreen.text(":",52,10);
}
void printHour(){
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(BCK_RED, BCK_GREEN, BCK_BLUE);
  TFTscreen.text(intToString(recent_dt.hour).c_str(),10,10);
  TFTscreen.stroke(RED, GREEN, BLUE);
  TFTscreen.text(intToString(dt.hour).c_str(),10,10);
}

void printMinute(){
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(BCK_RED, BCK_GREEN, BCK_BLUE);
  TFTscreen.text(intToString(recent_dt.minute).c_str(),70,10);
  TFTscreen.stroke(RED, GREEN, BLUE);
  TFTscreen.text(intToString(dt.minute).c_str(),70,10);
}

void printSecond(){
  TFTscreen.setTextSize(2);
  TFTscreen.stroke(BCK_RED, BCK_GREEN, BCK_BLUE);
  TFTscreen.text(intToString(recent_dt.second).c_str(),125,25);
  TFTscreen.stroke(RED, GREEN, BLUE);
  TFTscreen.text(intToString(dt.second).c_str(),125,25);
}

String intToString(int i){
  String result= String(i,10);
  if(i<10)result="0"+result;
  return result;
}

void printAlarm1(){
  RTCAlarmTime a = clock.getAlarm1();
  TFTscreen.stroke(RED, GREEN, BLUE);
  TFTscreen.setTextSize(1);
  TFTscreen.text("A1 ",10,85);
  TFTscreen.setTextSize(2);
  TFTscreen.text(intToString(a.hour).c_str(),25,85);
  TFTscreen.text(":",50,85);
  TFTscreen.text(intToString(a.minute).c_str(),60,85);
}

void printAlarm2(){
  RTCAlarmTime a = clock.getAlarm1();
  TFTscreen.stroke(RED, GREEN, BLUE);
  TFTscreen.setTextSize(1);
  TFTscreen.text("A2 ",10,105);
  TFTscreen.setTextSize(2);
  TFTscreen.text(intToString(a.hour).c_str(),25,105);
  TFTscreen.text(":",50,105);
  TFTscreen.text(intToString(a.minute).c_str(),60,105);
}

void printTemperature(){
  TFTscreen.stroke(RED, GREEN, BLUE);
  TFTscreen.setTextSize(2);
  TFTscreen.text("C 20",105,105);
}

