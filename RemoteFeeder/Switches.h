#ifndef SWITCHES_H
#define SWITCHES_H
#include <Arduino.h>

//define where your pins are

#define LATCH_PIN 7
#define CLOCK_PIN 6
#define DATA_PIN 5


//Define variables to hold the data 
//for shift register.
//starting with a non-zero numbers can help
//troubleshoot
byte switchVar1 = 72;  //01001000
byte isBitNum = 7;  //// always have the last switch on so we can validate the chip is working


byte resetBitNum=6;
byte feedBitNum = 5;
byte drainFillBitNum = 4;

byte feedDoor = 0;
byte dropBitNum = 2;
byte loadBitNum = 1;

//byte cBitNum = 1;
//byte lowBitNum = 0;


int dbFeedSwitch = 0;
int dbResetSwitch = 0;
int dbDrainFillSwitch = 0;
#endif
