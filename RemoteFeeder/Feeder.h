/*
  Wire colors:
    Serial: Green gnd, Brown is 2, Brown white is 3
    
    LED: 
    Switches
      Feed position: Orange/Oriange white
      Load position: Brown/Brown white.
      
      
    Motor Red to pin 3 on middel chip, Black to pin 6 on middel chip
    
    Drain
    Fill
    
*/

#ifndef FEEDER_H
#define FEEDER_H
#include <SimpleTimer.h>
#include "RGBLed.h" 
#define MOTOR_1_A 13
#define MOTOR_1_B 12 //
#define WATER_DEPTH A3

//celinodes are 11 and 12


#define SWITCH_TIMEOUT 5000 //Shut off motor if no switch was hit within 5 seconds. 
                            //if a switch breaks or for any reason fails to initiate 
                            //we do not want to brun out the motor
                            
#define STORE_DEPTH_PIN 8 //Range finder - how much food is left

int stuckRetry=5;
int fillWaitTime=200;
int motor_feed[] = {MOTOR_1_A, MOTOR_1_B};
boolean feeddooropen = false;
long waterlevel=0;
long foodstoredepth=0;

extern void logToCoordinator(String data);
extern RGBLed* getIndicator();
extern void setIndicator(const byte* color);

void(* resetFunc) (void) = 0;//declare reset function at address 0

#endif
