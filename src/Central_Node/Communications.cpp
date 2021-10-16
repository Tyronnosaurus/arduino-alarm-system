#include "Communications.h"


//SPI connections for nRF24l01+ module
const int CEpin = 48;  //Chip Enable
const int SCNpin = 49;  //Chip Select Not

//RF24Network parameters
const int CHANNEL = 100;
const uint16_t ADDRESS = 0;  //Central Node always has adress 0




//Constructor. The RF24 and RF24Network data members are initialized here, in the initializer list
Communications::Communications() : radio(CEpin,SCNpin),network(radio)
{

}



//Prepare the wireless network
void Communications::begin()
{
  //Bring up the RF network
  radio.begin();  
  network.begin(CHANNEL,ADDRESS);
}



/*
This is the brain of the Central Node communications. It checks for incoming messages, 
analyses them and performs corresponding actions (updating internal variables, sending 
messages to other nodes, etc.).
*/
void Communications::update()
{
  network.update();  //Must be called regularly
  
  ////
  //Things to do when a message arrives
  ////
  if(network.available()){
    
    RF24NetworkHeader inHeader;  //A header object is like the envelope of the message. It contains information about the 
    network.peek(inHeader);      //contents of the message so that we can know which datatype it contains (null, int, 
                                 //boolean, array...) before actually reading it
    printf_P(PSTR("\n----------------------------------------------------------------\n"));
    
  
  
    switch(inHeader.type)
    {
    
      /////////////////////////////////////
      ////Messages type A
      ////The user has pressed * on the Key Tray Node and it has sent a notification to the Central Node to activate the alarm
      /////////////////////////////////////
      case 'A':
      {
        //The Key Tray Node receives automatic acknowledgement that this message has arrived. There is no need to manually send acknowledgement
        network.read(inHeader,NULL,0);//Read the message. Even though it's empty we must take it out from the queue
        printf_P(PSTR("Alarm Activated"));
        Settings::setAlarmState(true);     
        break;
      }
      
      
   
      /////////////////////////////////////
      ////Messages type B
      ////The user has input a passcode (through numpad or RFID) in the Key Tray Node and it has been sent for confirmation
      /////////////////////////////////////
      case 'B':
      {
        byte receivedPasscode[PASSCODELENGTH];
        
        network.read(inHeader,&receivedPasscode,sizeof(receivedPasscode));
       
        printf_P(PSTR("Received passcode:\n  Passcode: "));
        for(int i=0;i<PASSCODELENGTH;i++) printf_P(PSTR("%u "),receivedPasscode[i]);
  
        bool accessGranted = Settings::isPasscodeInDatabase(receivedPasscode);
        if (accessGranted){
          printf_P(PSTR("--> Valid passcode. Sending confirmation... "));
          Settings::setAlarmState(false);
        }
        else printf_P(PSTR("--> Invalid passcode. Sending refusal... "));
  
        RF24NetworkHeader outHeader(inHeader.from_node, 'C'); //(to the same node, type)
        if(network.write(outHeader, &accessGranted, sizeof(bool))) printf_P(PSTR("sent."));
        break;
      }
      
      
      
      /////////////////////////////////////
      ////Messages type D
      ////The user has input a NEW passcode (through numpad or RFID) in the Key Tray Node and it has been 
      ////sent to be added to database
      /////////////////////////////////////
      case 'D':
      {
        byte receivedPasscode[PASSCODELENGTH];
        
        network.read(inHeader,&receivedPasscode,sizeof(receivedPasscode));
       
        printf("Received new passcode to add to database:\n  Passcode: ");
        for(int i=0;i<PASSCODELENGTH;i++) printf("%u ",receivedPasscode[i]);
  
        Settings::addNewPasscode(receivedPasscode);
        break;
      }
  
  
  
      /////////////////////////////////////
      ////Messages type E
      ////A Movement Detector Node has been triggered and it has sent this alert
      /////////////////////////////////////
      case 'E':
      {  
        network.read(inHeader,0,0); //Read the message. Even though it's empty we must take it out from the queue
        
        printf_P(PSTR("   / \\\n"));
        printf_P(PSTR("  / ! \\   A Movement Detector Node has been triggered!\n"));
        printf_P(PSTR(" /_____\\\n\n"));
        
        if (!Settings::isAlarmActivated()) printf_P(PSTR("Since the alarm is deactivated no further action is required."));
        else{
          printf_P(PSTR("Sending alert to Buzzer Node..."));
          RF24NetworkHeader outHeader(2, 'F'); //(receiver, type)
          uint8_t retries = 0;
          bool sent = false;
          
          while ((!sent)&&(retries<10)){
            printf_P(PSTR("\nSending... "));
            sent = network.write(outHeader,0,0);
            if (sent) printf_P(PSTR("sent."));
            else printf_P(PSTR("failure."));
            retries++;
          }    
          if (!sent) printf_P(PSTR("\nMake sure the Buzzer Node is powered on and in range."));
        }
        break;
      } 
      
      
      
      /////////////////////////////////////
      ////Messages type G
      ////A Window Node has been triggered and it has sent this alert
      /////////////////////////////////////
      case 'G':
      {
        network.read(inHeader,0,0); //Read the message. Even though it's empty we must take it out from the queue
        
        printf_P(PSTR("   / \\\n"));
        printf_P(PSTR("  / ! \\   A Window Node has been triggered!\n"));
        printf_P(PSTR(" /_____\\\n\n"));
        
        if (!Settings::isAlarmActivated()) printf_P(PSTR("Since the alarm is deactivated no further action is required."));
        else{
          printf_P(PSTR("Sending alert to Buzzer Node..."));
          RF24NetworkHeader outHeader(2, 'F'); //(receiver, type)
          uint8_t retries = 0;
          bool sent = false;
          
          while ((!sent)&&(retries<10)){
            printf_P(PSTR("\nSending... "));
            sent = network.write(outHeader,0,0);
            if (sent) printf_P(PSTR("sent."));
            else printf_P(PSTR("failure."));
            retries++;
          }    
          if (!sent) printf_P(PSTR("\nMake sure the Buzzer Node is powered on and in range."));
        }
        break;
      }
        
        
        
        
      /////////////////////////////////////
      ////Messages of type unknown
      /////////////////////////////////////
      default:
      {
        network.read(inHeader,NULL,0);  //Remove message from queue
        printf_P(PSTR("Careful! Another node has sent a message with a type that does not follow protocol. Revise its programming"));
        break;
      } 
    
    }
    
  }

}




