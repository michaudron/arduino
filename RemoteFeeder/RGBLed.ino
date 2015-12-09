#include "RGBLed.h"



RGBLed::RGBLed(int pinRed, int pinGreen, int pinBlue) {
  ledAnalog[0] = pinRed;
  ledAnalog[1] = pinGreen;
  ledAnalog[2] = pinBlue; 
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  setColor(BLACK); 
}

/* Sets the color of the LED to any RGB Value   
  led - (int array of three values defining the LEDs pins (led[0] = redPin, led[1] = greenPin, led[2] = bluePin))   
	color - (byte array of three values defing an RGB color to display (color[0] = new Red value, color[1] = new Green value, color[2] = new Red value
*/
void RGBLed::setColor(byte* color){
  CurrentColor[0] = color[0];
  CurrentColor[1] = color[1];
  CurrentColor[2] = color[2];
  
  for(int i = 0; i < 3; i++){     
     analogWrite(ledAnalog[i], 255 - color[i]);  
  }
}

/*

*/
void RGBLed::setColor(const byte* color){ 
	byte tempByte[] = {color[0], color[1], color[2]}; 
	setColor(tempByte);
        
}

/* Fades the LED from a start color to an end color at fadeSpeed   
  led - (int array of three values defining the LEDs pins (led[0] = redPin, led[1] = greenPin, led[2] = bluePin)) 
  startCcolor - (byte array of three values defing the start RGB color (startColor[0] = start Red value, startColor[1] = start Green value, startColor[2] = start Red value   
  endCcolor - (byte array of three values defing the finished RGB color(endColor[0] = end Red value, endColor[1] = end Green value, endColor[2] = end Red value   
  fadeSpeed - this is the delay in milliseconds between steps, defines the speed of the fade
*/ 
void RGBLed::fadeToColor(byte* startColor, byte* endColor, int fadeSpeed) {
  int changeRed = endColor[0] - startColor[0];                            
  int changeGreen = endColor[1] - startColor[1];                         
  int changeBlue = endColor[2] - startColor[2];                          
  int steps = max(abs(changeRed),max(abs(changeGreen), abs(changeBlue))); 
  for(int i = 0 ; i < steps; i++){    
    byte newRed = startColor[0] + (i * changeRed / steps);                 
    byte newGreen = startColor[1] + (i * changeGreen / steps);             
    byte newBlue = startColor[2] + (i * changeBlue / steps); 
    byte newColor[] = {newRed, newGreen, newBlue};                         
    setColor(newColor);                                              
//Set the LED to the calculated value
    delay(fadeSpeed);                                                     
 //Delay fadeSpeed milliseconds before going on to the next color  
  }  
  setColor(endColor);                                               
//The LED should be at the endColor but set to endColor to avoid rounding errors
}

/* 
  A version of fadeToColor that takes predefined colors (neccesary to allow const int pre-defined colors 
*/
void RGBLed::fadeToColor(const byte* startColor, const byte* endColor, int fadeSpeed){ 
 byte tempByte1[] = {startColor[0], startColor[1], startColor[2]};   
 byte tempByte2[] = {endColor[0], endColor[1], endColor[2]};   
 fadeToColor(tempByte1, tempByte2, fadeSpeed);
}
