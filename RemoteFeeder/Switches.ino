#include "Switches.h"

extern void feed(String args);
extern void reset(String args);


void initSwtiches() {
 //define pin modes
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT); 
  pinMode(DATA_PIN, INPUT);
  
}

void debounce() {
     debounceFeedSwitch();
     debounceResetSwitch();
     debounceDrainFillSwitch();
}

boolean isSwitch() {
  return getBit(switchVar1, isBitNum);
}


////// ----------------------------------------getBit
boolean getBit(byte myVarIn, byte whatBit) {
  boolean bitState;
  bitState = myVarIn & (1 << whatBit);
  return bitState;
}

////// ----------------------------------------shiftIn function
///// just needs the location of the data pin and the clock pin
///// it returns a byte with each bit in the byte corresponding
///// to a pin on the shift register. leftBit 7 = Pin 7 / Bit 0= Pin 0

byte shiftIn(int myDataPin, int myClockPin) { 
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);
//we will be holding the clock pin high 8 times (0,..,7) at the
//end of each time through the for loop

//at the begining of each loop when we set the clock low, it will
//be doing the necessary low to high drop to cause the shift
//register's DataPin to change state based on the value
//of the next bit in its serial information flow.
//The register transmits the information about the pins from pin 7 to pin 0
//so that is why our function counts down
  for (i=7; i>=0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(0.2);
    temp = digitalRead(myDataPin);
    if (temp) {
      pinState = 1;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    else {
      //turn it off -- only necessary for debuging
     //print statement since myDataIn starts as 0
      pinState = 0;
    }

    //Debuging print statements
    //Serial.print(pinState);
    //Serial.print("     ");
    //Serial.println (dataIn, BIN);

    digitalWrite(myClockPin, 1);

  }
  //debuging print statements whitespace
  //Serial.println();
  //Serial.println(myDataIn, BIN);
  return myDataIn;
}

void debounceFeedSwitch() {
  boolean feedButton = getBit(switchVar1, feedBitNum);
  if(dbFeedSwitch == 0  && feedButton) {
    dbFeedSwitch = 1;
  } else if(dbFeedSwitch == 1 && !feedButton) {
    Serial.println("FeedButton Released");
    dbFeedSwitch=0;
    feed("");
    
  }
}
void debounceDrainFillSwitch() {
  boolean button = getBit(switchVar1, drainFillBitNum);
  if(dbDrainFillSwitch == 0  && button) {
    dbDrainFillSwitch = 1;
  } else if(dbDrainFillSwitch == 1 && !button) {
    Serial.println("Drain Fill Released");
    dbDrainFillSwitch=0;
    //feed("");
  }
}

void debounceResetSwitch() {
  boolean button = getBit(switchVar1, resetBitNum);
  if(dbDrainFillSwitch == 0  && button) {
    dbResetSwitch = 1;
  } else if(dbResetSwitch == 1 && !button) {
    dbResetSwitch=0;
    reset("");
  }
}

void checkSwitches() {
  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(LATCH_PIN,1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite(LATCH_PIN,0);

  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first 
   switchVar1 = shiftIn(DATA_PIN, CLOCK_PIN);
  

 //  Serial.println(switchVar1,BIN); 
 //  delay(1000);
  
}




