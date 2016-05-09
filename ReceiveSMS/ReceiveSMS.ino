/*
 SMS receiver

 This sketch, for the Arduino GSM shield, waits for a SMS message
 and displays it through the Serial port.

 Circuit:
 * GSM shield attached to and Arduino
 * SIM card that can receive SMS messages

 created 25 Feb 2012
 by Javier Zorzano / TD

 This example is in the public domain.

 http://www.arduino.cc/en/Tutorial/GSMExamplesReceiveSMS

*/
#include <Console.h>
// include the GSM library
#include <GSM.h>

// PIN Number for the SIM
#define PINNUMBER "1973"

// initialize the library instances
GSM gsmAccess(true);
GSM_SMS sms;
GSMScanner scannerNetworks;

// Array to hold the number a SMS is retreived from
char senderNumber[20];

void setup() {
   // initialize serial communication:
  Bridge.begin();
  Console.begin();

  while (!Console){
    ; // wait for Console port to connect.
  }

  Console.println("SMS Messages Receiver");
  //scannerNetworks.begin();

  // connection state
  boolean notConnected = true;

  // Start GSM connection
  while (notConnected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      notConnected = false;
    } else {
      Console.println("Not connected");
      delay(1000);
    }
  }

  Console.println("GSM initialized");
  Console.println("Waiting for messages");
}

void loop() {
  char c;
  String SMS_Msg = "";


  // Still msg to read 
  boolean StillMsg = true;


  while (StillMsg) {

  // If there are any SMSs available()
  if (sms.available()) {
    Console.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Console.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#') {
      Console.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while (c = sms.read()) {
      SMS_Msg +=String(c);
//      Console.print(c);
    }
    
    SMS_Msg.toLowerCase();
    Console.print(SMS_Msg);
    if (SMS_Msg.startsWith("?"))
    {
    Console.println("\nThis is a question");
    }
    SMS_Msg = "";
    Console.println("\nEND OF MESSAGE");

    // Delete message from modem memory
    sms.flush();
    Console.println("MESSAGE DELETED");
  }
  else
  StillMsg = false;
}

//  gsmAccess.shutdown();
  ///Console.println("GSM Shutdown");
  delay(1000);

}


