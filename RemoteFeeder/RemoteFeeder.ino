/**
 * Last update: 12/17/2014
 */

#include <SoftwareSerial.h>//see if this works when moved to library.  headr include not working unless I add this here

#include <SimpleTimer.h>
#include "XBeeAt.h"
#include "Switches.h"
#include "Feeder.h"
#include "RGBLed.h"



XBeeAt XBee(2,3);
RGBLed indicator(A2,A1,A0);

String lineData;
int pollCounter=0;


void setup() {
  // put your setup code here, to run once:

 Serial.begin(9600);
 XBee.begin(9600);
  initFeeder();
 initSwtiches();
 //initFeeder();

 Serial.println(F("Checking XBee"));
 indicator.setColor(YELLOW);
 
 
 if(XBee.waitReady()) {
   Serial.println(F("XBee is ready!"));
   setIndicator(GREEN);
   Serial.println(F("Running"));
 } else {
   Serial.println(F("Could not communicat with the Coordinator. Try to reset the feeder."));
   setIndicator(BLUE);
 }
 XBee.addCommand("FEED",feed);
 XBee.addCommand("setName",setName);
 XBee.addCommand("sendAllMonitors", sendAllMonitors);
 XBee.addCommand("reset", reset);
 //XBee.addCommand("test", testFeed);
}

void loop() {
      
      checkSwitches();
      debounce();//do this here so it only happens when we want not on every check
      lineData = XBee.check();
      
      pollCounter++;
      if(pollCounter == 10) {
        pollCounter=0;
        Serial.println("Poll");
        (void)isFeederDoorOpen();
        checkStoreDepth();
        checkWaterLevel();
      }
      
      
      if(lineData != NULL && lineData.length() > 0) {
        Serial.print(F("XBEE: ["));
        Serial.print(lineData);
        Serial.println(F("]"));

        if(!XBee.isReady()) {
          if(XBee.waitReady()) {
            setIndicator(GREEN);
          }
        }
      }    
}

void testFeed(String args) {
 for(int i = 0 ; i < 20;i++) { 
   Serial.print("Test feeding: ");
   Serial.println(i);
   feed(args);
   delay(10000);
 }
}
RGBLed* getIndicator() {
    return &indicator;
}

void setIndicator(const byte* color) { 
  if(indicator.CurrentColor[0] != color[0] ||
     indicator.CurrentColor[1] != color[1] ||
     indicator.CurrentColor[2] != color[2]) {
       XBee.send("statuslight|" + String(color[0]) + "," + String(color[1]) + "," + String(color[2]));
  }
  
  indicator.setColor(color);
}

void logToCoordinator(String data) {
  XBee.send(data);
}

void getName(String args) {
  XBee.send("name|" + XBee.getName());
}

void setName(String args) {
  if(XBee.setName(args)) {
    XBee.send("myName|" + XBee.getName());
  }
}


void checkStoreDepth() {
  int lvl = getFoodStoreDepth();
  if(lvl != foodstoredepth) {
    foodstoredepth = lvl;
     XBee.send("foodstore|" + String(lvl));
  }
}
void checkWaterLevel() {
  int lvl = getWaterLevel();
  
  if(lvl!= waterlevel) {
    waterlevel = lvl;
    XBee.send("waterlevel|" + String(lvl));
  }
}

void reset(String args) {
  resetFunc();
}

void sendAllMonitors(String args) {
 getName(args);
 XBee.send("foodstore|" + String());
 XBee.send("fooddoor|" + String((isFeederDoorOpen()?"open":"closed")));
 byte* tmp = indicator.getColor();
 XBee.send("statuslight|" + String(tmp[0]) + "," + String(tmp[1]) + "," + String(tmp[2]));
 XBee.send("waterlevel|" + String(getWaterLevel()));
  XBee.send("dispensor|" + String((isDrowAtDrop()?"drop":isDrowAtLoad()?"loading":"unknown")));
}




