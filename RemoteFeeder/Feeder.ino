#include "Feeder.h"
#include "Switches.h"
// -- Pin change interrupt

SimpleTimer feederTimer;
boolean feeding=false;
boolean timedout=false;


void initFeeder() {
   motorOff();
  for(int i = 0; i < 2; i++){
    pinMode(motor_feed[i], OUTPUT);
  }
  pinMode(WATER_DEPTH,INPUT);
  motorOff();
}

int getFillWaitTime() {
   return fillWaitTime;
}
int getStuckRetryCount() {
  return stuckRetry;
}
void turnMotorForward() {
  digitalWrite(motor_feed[0], HIGH); 
  digitalWrite(motor_feed[1], LOW); 
}
void turnMotorBack() {
  digitalWrite(motor_feed[0], LOW); 
  digitalWrite(motor_feed[1], HIGH); 
}
void motorOff() {
  digitalWrite(motor_feed[0], LOW); 
  digitalWrite(motor_feed[1], LOW); 
}


boolean isDrowAtDrop() {
  return getBit(switchVar1, dropBitNum);
}

boolean isDrowAtLoad() {
  return getBit(switchVar1, loadBitNum);
}


boolean isFeederDoorOpen() {
  //When the door is closed we have power throught the switch
  //if we check for the bit to be off the door could be open and the switch brocken.
  //this way they can't remove the door switch and have this run.
  if(getBit(switchVar1, feedDoor)) {
    if(feeddooropen == true) {
      logToCoordinator("fooddoor|closed");
      feeddooropen = false;
    }
  } else {
    if(feeddooropen == false) {
      logToCoordinator("fooddoor|open");
      feeddooropen = true;
    }
  }
  return feeddooropen;
}




void waitForDoorToClose() {
  if(isFeederDoorOpen()) {
    //get a pointer to the current color
    byte* curColor = getIndicator()->getColor();
    //make a copy so we don't use the pointer as it 
    //will change as soon as we change the color
    byte tempColor[] = {curColor[0], curColor[1], curColor[2]};
    
    getIndicator()->setColor(CYAN);
    
    logToCoordinator("fooddoor|open");
     while(isFeederDoorOpen()) {
         checkSwitches(); 
         delay(1000);
     }
     getIndicator()->setColor(tempColor);
     logToCoordinator("fooddoor|closed");
  }
}

void motorTimeout() {
  Serial.println("Feeder switch timed out...");
  motorOff();
  timedout=true;
}

void stopFeed(String msg, const byte *color) {
  if(color != NULL) {
    setIndicator(color);
  }

  if(msg.length() > 0) {
    logToCoordinator(msg);
  }
  timedout=false;
  feeding=false;
}

void gotoLoadFood() {
   int timeOut;
   timedout=false;
   if(!isDrowAtLoad()) {
     turnMotorForward();
     timeOut = feederTimer.setTimeout(SWITCH_TIMEOUT, motorTimeout); 
     while(!isDrowAtLoad()) {
         if(timedout) {
           stopFeed("msg|ERROR - Issue feeding. Timed out waiting for the load switch.", RED);
           return;
         }
         feederTimer.run();
         if(isFeederDoorOpen()) {
          motorOff();
          feederTimer.disable(timeOut);
          waitForDoorToClose();
          turnMotorForward(); 
          feederTimer.enable(timeOut);
         }
         checkSwitches(); 
     }
     feederTimer.deleteTimer(timeOut);
  }
  motorOff();
}


//Commands
void feed(String args) {
if(feeding)
  return;

 feeding=true;
 checkSwitches();

 if(!isSwitch()) {
   stopFeed("msg|ERROR No switches found, May have a missing or bad chip.", RED);
   return;
 }
 
 if(isFeederDoorOpen()) {
   waitForDoorToClose();
 }
 gotoLoadFood();
  
  Serial.print(F("Let fill for "));
  Serial.print(getFillWaitTime());
  Serial.println(F(" seconds"));
  delay(getFillWaitTime());
  //delay(500);
  checkSwitches();
  turnMotorBack();
  int retryCount=0;
  int timeOut = feederTimer.setTimeout(SWITCH_TIMEOUT, motorTimeout);
  while(!isDrowAtDrop()) {
    if(timedout ) {
      Serial.println("Stuck will retry");
      retryCount++;
      if(retryCount>3) {      
        stopFeed("msg|ERROR - Issue feeding. Timed out waiting for the drop switch.", RED);
        return;
      }
      gotoLoadFood();
      delay(getFillWaitTime());
      timeOut = feederTimer.setTimeout(SWITCH_TIMEOUT, motorTimeout);
      checkSwitches();
      turnMotorBack();
    } 
    feederTimer.run();
    if(isFeederDoorOpen()) {
       motorOff();
       feederTimer.disable(timeOut);
       waitForDoorToClose();
       turnMotorBack();
       feederTimer.enable(timeOut);
    }
    
    checkSwitches(); 
  }
  motorOff();
  feederTimer.deleteTimer(timeOut);
  stopFeed("gavefood|" + String(getFoodStoreDepth()), GREEN);
  
}

long getWaterLevel() {
  long lvl = analogRead(WATER_DEPTH);
  
  if(lvl < 200) {
    lvl = 100;
  } else if(lvl < 300) {
    lvl=75;
  } else if(lvl < 400) {
    lvl=65;
  } else if(lvl < 500) {
    lvl=50;
  } else if(lvl < 600) {
    lvl=25;
  } else {
    lvl=0;
  }
  
  return lvl;
}

long getFoodStoreDepth() {
  long duration;
  pinMode(STORE_DEPTH_PIN, OUTPUT);
  digitalWrite(STORE_DEPTH_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(STORE_DEPTH_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(STORE_DEPTH_PIN, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(STORE_DEPTH_PIN, INPUT);
  duration = pulseIn(STORE_DEPTH_PIN, HIGH);

  // convert the time into a distance
  return (100 - (duration / 74 / 2) * 5);
  //inches = microsecondsToInches(duration);
  //cm = microsecondsToCentimeters(duration);
    // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  // microseconds / 74 / 2;
  
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  //microseconds / 29 / 2;
  
}
