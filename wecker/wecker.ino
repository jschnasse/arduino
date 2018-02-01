
#include <SPI.h>
#include <TFT.h>
#include <Wire.h>
#include <DS3231.h>

//TFT
#define CS   10
#define DC   9
#define RESET  8  
#define SCLK 13
#define MOSI 11

//Clock
#define SDA "sda"
#define SCL "scl"

//Buttons
#define ROT_CLK 2
#define ROT_DT 3
#define ROT_SW 4
#define A1_BTN 4
#define A2_BTN 5

//Buzzer
#define BUZZ 12

//TERMO
#define TERM 6

DS3231 clock;
RTCDateTime dt;
RTCDateTime recent_dt;

TFT TFTscreen = TFT(CS, DC,RESET);
byte bgcolor[3] = {255,255,255};
byte textcolor[3] = {0,0,0};
byte markercolor[3] = {64,64,64};


void setup() {
  Serial.begin(9600);
  TFTscreen.begin();
  TFTscreen.background(bgcolor[0], bgcolor[1], bgcolor[2]);
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
  DS3231_alarm1_t mode1 = 0b00001000;//DS3231_MATCH_H_M 
  clock.setAlarm1(1,11,29,0,mode1,true);
  DS3231_alarm2_t mode2 = 0b00001000;//DS3231_MATCH_H_M 
  clock.setAlarm2(1,14,30,mode2,true);
  attachInterrupt (0,isr,FALLING);
}

void loop() {
  recent_dt=dt;
  dt = clock.getDateTime();
  printTime();
  printAlarm1();
  printAlarm2();
  printTemperature();
  checkForAlarm();
  checkForUserInput();
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
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.text(":",52,10);
}
void printHour(){
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(bgcolor[0], bgcolor[1], bgcolor[2]);
  TFTscreen.text(intToString(recent_dt.hour).c_str(),10,10);
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.text(intToString(dt.hour).c_str(),10,10);
}

void printMinute(){
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(bgcolor[0], bgcolor[1], bgcolor[2]);
  TFTscreen.text(intToString(recent_dt.minute).c_str(),70,10);
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.text(intToString(dt.minute).c_str(),70,10);
}

void printSecond(){
  TFTscreen.setTextSize(2);
  TFTscreen.stroke(bgcolor[0], bgcolor[1], bgcolor[2]);
  TFTscreen.text(intToString(recent_dt.second).c_str(),125,25);
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.text(intToString(dt.second).c_str(),125,25);
}

String intToString(int i){
  String result= String(i,10);
  if(i<10)result="0"+result;
  return result;
}

void printAlarm1(){
  RTCAlarmTime a = clock.getAlarm1();
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.setTextSize(1);
  TFTscreen.text("A1 ",10,85);
  TFTscreen.setTextSize(2);
  TFTscreen.text(intToString(a.hour).c_str(),25,85);
  TFTscreen.text(":",50,85);
  TFTscreen.text(intToString(a.minute).c_str(),60,85);
}

void printAlarm2(){
  RTCAlarmTime a = clock.getAlarm2();
  TFTscreen.stroke(textcolor[0], textcolor[1], textcolor[2]);
  TFTscreen.setTextSize(1);
  TFTscreen.text("A2 ",10,105);
  TFTscreen.setTextSize(2);
  TFTscreen.text(intToString(a.hour).c_str(),25,105);
  TFTscreen.text(":",50,105);
  TFTscreen.text(intToString(a.minute).c_str(),60,105);
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

int RotaryPosition=0;
volatile boolean TurnDetected;  // Muss für unterbrechungen flüchtig sein
volatile boolean rotationdirection;  // CW- oder CCW-Drehung
int PrevPosition;     // Vorherige Drehposition Wert zur Prüfung der Genauigkeit
int StepsToTake;      // Wie viel der Cursor sich bewegen muss
bool select=true;

void checkForUserInput(){
  if (!(digitalRead(ROT_SW))) {   // Überprüfen Sie, ob die Taste gedrückt wird
  cursorsel(); 
  }

  //Läuft, wenn Rotation erkannt wurde
  if (TurnDetected)  {
    PrevPosition = RotaryPosition; // Vorherige Position in Variable speichern
    if (rotationdirection) {
      RotaryPosition=RotaryPosition-1;} // Senkt die Position um 1
    else {
      RotaryPosition=RotaryPosition+1;} // Erhöht die Position um 1

    TurnDetected = false;  //NICHT wiederholen IF-Schleife, bis neue Rotation erkannt wird
    // Welche Richtung Schrittmotor bewegen
    if ((PrevPosition + 1) == RotaryPosition) { // Motor CW bewegen
      StepsToTake=50;
      cursormv(StepsToTake);
    }

    if ((RotaryPosition + 1) == PrevPosition) { //Motor CCW verschieben
      StepsToTake=-50;
      cursormv(StepsToTake);
    }
  }

}
void isr ()  {
  delay(4);  // Verzögerung für Entprellung
  if (digitalRead(ROT_CLK))
    rotationdirection= digitalRead(ROT_DT);
  else
    rotationdirection= !digitalRead(ROT_DT);
  TurnDetected = true;
}
void cursormv(int move){
  Serial.print("Move: ");
  Serial.println(move);
  Serial.print("RotaryPosition: ");
  Serial.println(RotaryPosition);
  Serial.print("StepsToTake: ");
  Serial.println(StepsToTake);
}

void cursorsel(){
  select=!select;
  Serial.print("SELECT: ");
  Serial.println(select);
}
void beep(){

}



