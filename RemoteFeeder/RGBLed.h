#ifndef RGBLED_H
#define RGBLED_H
#include <Arduino.h>

const byte RED[] = {255, 0, 0}; 
const byte ORANGE[] = {83, 4, 0}; 
const byte YELLOW[] = {255, 255, 0}; 
const byte GREEN[] = {0, 255, 0}; 
const byte BLUE[] = {0, 0, 255}; 
const byte INDIGO[] = {4, 0, 19}; 
const byte VIOLET[] = {23, 0, 22}; 
const byte CYAN[] = {0, 255, 255}; 
const byte MAGENTA[] = {255, 0, 255}; 
const byte WHITE[] = {255, 255, 255}; 
const byte BLACK[] = {0, 0, 0}; 
const byte PINK[] = {158, 4, 79};


class RGBLed {
  private:
    int ledAnalog[3];

  public:
    byte CurrentColor[3];
    RGBLed(int pinRed, int pinGreen, int pinBlue);
    ~RGBLed() {};
    byte* getColor() {return CurrentColor;};
    void setColor(const byte* color);
    void setColor(byte* color);
    void fadeToColor(byte* startColor, byte* endColor, int fadeSpeed);
    void fadeToColor(const byte* startColor, const byte* endColor, int fadeSpeed);
};



#endif
