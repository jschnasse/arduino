#ifndef USERINPUT_h
#define USERINPUT_h
#include <DS3231.h>
#include "ScreenSegment.h"

typedef enum {
  SELECT = 0b00000001,
  SET = 0b00000010,
  SAVE = 0b00000100
} InputMode;

struct UserInput{
    RTCDateTime lastUserAction;
    InputMode mode;
    ScreenSegment* cursorPosition;   
    ScreenSegment* recentPosition;  
};

#endif
