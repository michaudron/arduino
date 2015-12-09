#include "XBeeAt.h"

void XBeeAt::send(String data) {
  print(getAddress());
  print(F("|coordinator|"));
  print(data);
  Serial.println("Sending " + data);
  write(XBEE_RETURN);
  delay(10);
}

bool XBeeAt::waitReady(int timeout) {
 //XBee takes a few seconds to be ready
  delay(timeout);
  int count=0;
  //Ping the coordinator and make sure can connect
  
  while(!enterCommandMode()) {
    delay(1000);
    count++;
    if(count == XBEE_RETRY_COUNT) {
       imready=false;
       return false;
    }
  }
  
  myName = sendATCMD("NI");
  myAddress = sendATCMD("SL");  
 (void) sendATCMD("CN"); //exit command mode
  if(myName.equals(XBEE_TIMEOUT_ERR) || myAddress.equals(XBEE_TIMEOUT_ERR)) {
    myName = "";
    myAddress = "";
    imready=false;
    return false;
  }
  Serial.println("Address: " + myAddress + "  Name: " + myName);
  //Ping the coordinator and make sure can connect
  count=0;
  while (!sendPing()) {
       delay(1000);
       count++;
       if(count == XBEE_RETRY_COUNT) {
         imready=false;
         return false;
       }
       
  }
  imready = true;
  return true;
}
void XBeeAt::setReady(bool val) {
  imready=val;
}
bool XBeeAt::setName(String name) {
bool rc=false;
  if(enterCommandMode()) {
      String resp = sendATCMD("NI" + name);
      if(resp.equals("OK")) {
        myName=name;
        rc=true;
        (void) sendATCMD("WR"); //exit command mode
      }
      (void) sendATCMD("CN"); //exit command mode
      Serial.print(F("My name changed to: ")); Serial.println(myName);
  } 
  return rc;
}

String XBeeAt::getName() {
  if(myName != NULL && myName.length() > 0) {
    return myName;
  }
  waitReady();
  
  return (myName==NULL?"":myName);
}

String XBeeAt::getAddress() {
  if(myAddress != NULL  && myAddress.length() > 0) {
    return myAddress;
  }
  waitReady();
  return (myAddress==NULL?"":myAddress);
}

String XBeeAt::waitForResponse(bool forme,int timeout) {
  int count=0;
  String incomming;
  do {
    while(!available()) {
       count++;
       if(count > (timeout * 10)) {
          Serial.println(XBEE_TIMEOUT_ERR);
          return XBEE_TIMEOUT_ERR;
       }
       delay(100);
    }
    incomming = getIncomming();
  } while(forme && (!splitResponse(2,incomming).equals(getAddress()) && !splitResponse(2,incomming).equals("ALL")));
  
  return forme?splitResponse(3,incomming):incomming;
}

bool XBeeAt::enterCommandMode(int timeout) {
  print("+++");
  if(waitForResponse(false,timeout).equals("OK")) {
    return true;
  }
  Serial.println(F("Faild to enter command mode"));
  return false;
}

String XBeeAt::getIncomming() {
  String newLine;
   // see if there's incoming serial data:
  while (available() > 0) {
    // read the oldest byte in the serial buffer:
    int oneByte = read();
    if(oneByte != 13) {
       newLine.concat((char)oneByte);
    } else {
      break;
    }
    delay(1);
  }
  return newLine; 
}

String XBeeAt::check() {
  String tmp = getIncomming();
  if(tmp != NULL) {
    String toAdder = splitResponse(2,tmp);
    if(toAdder.equals(getAddress()) || toAdder.equals("ALL")) {
      String cmdName = splitResponse(3,tmp);
      for(int i = 0; i < XBEE_COMMAND_COUNT;i++) {
        if(commands[i].name.equals(cmdName)) {
          commands[i].call(splitResponse(4,tmp));   
          break;
        }
      }
    } else {
      tmp = "";
    }
  }
  return tmp;
}

void XBeeAt::addCommand(String name, void (*call)(String args)) {
  if(addedCommands == XBEE_COMMAND_COUNT) {
    Serial.print(F("MAX commands used... Up the defined XBEE_COMMAND_COUNT ["));
    Serial.print(XBEE_COMMAND_COUNT);
    Serial.println(F("]"));
  }
  
  commands[addedCommands].name = name;
  commands[addedCommands].call = call;
  addedCommands++;
}



bool XBeeAt::sendPing() {
     send("ping");
     return waitForResponse(true).equals("pong");
}


String XBeeAt::sendATCMD(String cmd) {
   print("AT" + cmd); write(0xD);
   return waitForResponse();
}

void XBeeAt::showInfo() {
  if(enterCommandMode()) {
    Serial.print(F("Node                   : ")); Serial.println(sendATCMD("NI"));
    Serial.print(F("PAN ID                 : ")); Serial.println(sendATCMD("ID"));
    Serial.print(F("Serial Number High     : ")); Serial.println(sendATCMD("SH"));
    Serial.print(F("Serial Number Low      : ")); Serial.println(sendATCMD("SL"));
    Serial.print(F("Net 16bit address      : ")); Serial.println(sendATCMD("MY"));
    Serial.print(F("Operating PAN ID       : ")); Serial.println(sendATCMD("OP"));
    Serial.print(F("Operating Channel      : ")); Serial.println( sendATCMD("CH"));
    Serial.print(F("Number of Children left: ")); Serial.println(sendATCMD("NC"));
    Serial.print(F("Destination High Bit   : ")); Serial.println(sendATCMD("DH"));
    Serial.print(F("Destination Low Bit    : ")); Serial.println(sendATCMD("DL"));
    
    Serial.print(F("Exit                   : ")); Serial.println(sendATCMD("CN"));
    
  } else {
    Serial.println(F("FAILD to enter AT command mode"));
  }
}

String XBeeAt::splitResponse(int index, String data) {
  int items=0;
  int commaPosition=0;
  String value="";
  do {
      items++;
      commaPosition = data.indexOf('|');
      if(commaPosition != -1)
      {
        value = data.substring(0,commaPosition);
        data = data.substring(commaPosition+1, data.length());
        if(items >= index) {
          break;
        }
      } else {
        if(items == index) {
          value= data;
        }
      }
   } while(commaPosition >=0);
   return value;
}



