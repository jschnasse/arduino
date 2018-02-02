#ifndef SCREENSEGMENT_H
#define SCREENSEGMENT_H

class ScreenSegment{
  public:
    ScreenSegment(String name){
      time=recentTime=0;
      setName(name);
      };
    void setScreenRect(uint8_t x,uint8_t y,uint8_t width,uint8_t height){
      rect[0]=x;
      rect[1]=y,
      rect[2]=width;
      rect[3]=height;
    }
    uint8_t* getScreenRect(){
      return rect;
    }
    void setTextPos(uint8_t x,uint8_t y){
      textPos[0]=x;
      textPos[1]=y;
    }
    uint8_t* getTextPos(){
      return textPos;
    }

    void setTime(uint8_t t){
      recentTime=time;
      time=t;
    }

    uint8_t getTime(){
      return time;
    }
    uint8_t getRecentTime(){
      return recentTime;
    }
    String getTimeAsString(){
     return getTimeString(time);
    }

    String getRecentTimeString(){
      return getTimeString(recentTime);
    }
    
    void setSize(uint8_t s){
      size=s;
    }

    uint8_t getSize(){
      return size;
    }
    String getName(){
      return name;
    }
    void setName(String n){
      name=n;
    }
    uint8_t getUpperLimit(){
      return upperLimit;
    }
    void setUpperLimit(uint8_t l){
      upperLimit = l;
    }
  private:
    String getTimeString(uint8_t t){
      String result= String(t,10);
      if(t<10)result="0"+result;
      return result;
    }
    uint8_t rect[4];
    uint8_t textPos[2];
    uint8_t time;
    uint8_t recentTime;
    uint8_t size;
    String name;
    uint8_t upperLimit;
};

#endif
