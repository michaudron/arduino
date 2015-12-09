#ifndef XBEEAT_H
#define XBEEAT_H
#include <SoftwareSerial.h>

#ifndef XBEE_COMMAND_COUNT
#define XBEE_COMMAND_COUNT 10
#endif

#ifndef XBEE_RETRY_COUNT
#define XBEE_RETRY_COUNT 5 //times
#endif


#ifndef XBEE_TIMEOUT
#define XBEE_TIMEOUT 5 //seconds
#endif

#define XBEE_TIMEOUT_ERR "Time Out"

#ifndef XBEE_SETUP_TIMEOUT
#define XBEE_SETUP_TIMEOUT 5000
#endif

#define XBEE_RETURN 0xD

struct xbee_command {
 String name;
 void (*call)(String args);
};

struct xbee_packet {
  String from;
  String to;
  String command;
  String args;
 // String reserve;
};

//from|to|cmd|args1,arg2,...



class XBeeAt : public SoftwareSerial {
  private:
    String myAddress;
    String myName;
    bool imready;
    int addedCommands;
    xbee_command commands[XBEE_COMMAND_COUNT];
    bool sendPing();
    
  public:
    XBeeAt(int rx, int tx) : SoftwareSerial(rx,tx) {};
    String getIncomming();
    void addCommand(String name, void (*call)(String args));
    String check();
    bool enterCommandMode(int timeout=XBEE_TIMEOUT);
    String waitForResponse(bool forme=false,int timeout=XBEE_TIMEOUT);
    String sendATCMD(String cmd);
    void send(String data);
    void showInfo();
    String getAddress();
    String getName();
    bool setName(String name);
    bool isReady() {return imready;};
    void setReady(bool val);
    bool waitReady(int timeout=XBEE_SETUP_TIMEOUT);
  String splitResponse(int index, String data);
};

#endif
