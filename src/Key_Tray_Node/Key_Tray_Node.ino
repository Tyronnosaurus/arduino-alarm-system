/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This code has been developed for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.

  This is the main sketch for the Key Tray Node. It has a numpad and an RFID reader. With it, 
  the user can activate the alarm remotely, deactivate it by introducing a passcode (either by numpad or RFID tag), 
  and add new passcodes.
  
  It also has a green and a red LEDs (to let the user know when the alarm is activated or deactivated), and a piezo 
  that can ring different notes (for user feedback whenever he interacts with the device)
*/


///////////////////////////////////////////////////////////
////INCLUDED LIBRARIES/////////////////////////////////////
///////////////////////////////////////////////////////////
#include "Arduino.h"  //Needed for printf.h
#include <printf.h>  //Provides with the function printf() (very useful for debugging)

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

#include <MFRC522.h>  //RFID library

#include <Keypad.h>
#include "Pitches.h"

#include "Sounds.h"

///////////////////////////////////////////////////////////
////CONSTANTS//////////////////////////////////////////////
///////////////////////////////////////////////////////////

#define PASSCODELENGTH 6

////Related to numpad
static const byte ROWS = 4;
static const byte COLS = 3;
byte rowPins[ROWS] = {5, 6, 7, 8};
byte colPins[COLS] = {2, 3, 4};
char keys[ROWS][COLS] =
 {{'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}};


////Related to RFID
#define SS_PIN 32  //Chip Select or SDA
#define RST_PIN 33  //RST


////Related to nRF24
const int CEpin = 48;  //Chip Enable
const int SCNpin = 49;  //Chip Select Not
const int CHANNEL = 100;
const uint16_t ADDRESS = 3;


////Related to LEDS
const int REDLEDpin = 11;
const int GREENLEDpin = 12;

///////////////////////////////////////////////////////////
////GLOBAL VARS////////////////////////////////////////////
///////////////////////////////////////////////////////////
//Numpad instance
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//RFID reader instance 
MFRC522 mfrc522(SS_PIN, RST_PIN);

//Objects for wireless communications
RF24 radio(CEpin,SCNpin);
RF24Network network(radio);

////Other useful variables
uint8_t state;  //Stores current state of the state machine
byte passcode[PASSCODELENGTH];  //Stores the passcode input by the user
uint8_t count = 0;  //Tracks how many digits have been input

///////////////////////////////////////////////////////////
///////SETUP///////////////////////////////////////////////
///////////////////////////////////////////////////////////
void setup()
{
  ////Setup Serial communication for debugging
  Serial.begin(57600);
  printf_begin();
  printf_P(PSTR("Starting...\n"));
  
  ////Setup I/O pins
  pinMode(REDLEDpin,OUTPUT);
  pinMode(GREENLEDpin,OUTPUT);
  digitalWrite(REDLEDpin,LOW);
  digitalWrite(GREENLEDpin,HIGH);
  
  ////Bring up the RF network
  radio.begin();  
  network.begin(CHANNEL,ADDRESS);  //Also prints radio properties
  
  ////Setup the RFID reader
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); //Set Antenna Gain to Max. This will increase reading distance
  
  ////Setup the State Machine
  state = 1;
  printf_P(PSTR("\n--Advancing to state 1 (Alarm deactivated)--\nPress * to activate alarm, or press # to add a new passcode.\n"));
}



///////////////////////////////////////////////////////////
///////LOOP////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void loop()
{ 
  char pressedKey = keypad.getKey();
  network.update(); 
 
  
  //This module works as a state machine. The variable 'state' indicates the current state. 
  //Refer to the state machine diagram for a better understanding of the program flow
  switch(state){
    
    /////////////STATE 1//////////////
    //// Idle (alarm deactivated)/////
    //////////////////////////////////
    case 1:
      if (pressedKey == '*') {  //Check if the Activation key (*) has been pressed
        Sounds::KeyPressed();
        if (sendActivationNotice()){
          state = 2; 
          printf_P(PSTR("\n--Advancing to state 2 (Alarm activated)--\nIntroduce a 6 character long passcode. Press # to clear.\nPasscode: "));
          Sounds::AlarmActivated();
          digitalWrite(REDLEDpin,HIGH);
          digitalWrite(GREENLEDpin,LOW);
        }
      }
      
      else if (pressedKey == '#') {  //Check if the Add New Passcode key (#) has been pressed
        Sounds::AddingNewPasscode();
        state = 4; 
        printf_P(PSTR("\n--Advancing to state 4--\nIntroduce a new 6 character long passcode to add to database. Press # to clear.\nNew passcode: "));
        digitalWrite(REDLEDpin,HIGH);
        digitalWrite(GREENLEDpin,HIGH);
      }
      
      break;
      
      
    /////////////STATE 2//////////////
    ////Idle (alarm activated)
    //////////////////////////////////
    case 2:
    
      if(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()){ //Check if there is a new card and if it is readable
        Sounds::RFIDdetected();
        getRFIDpasscode();
        count = PASSCODELENGTH;
      }
    
      else if ((pressedKey >= '0') && (pressedKey <= '9')){  //Check if a number has been pressed and add it to passcode
        Sounds::KeyPressed();
        printf_P(PSTR("%c "),pressedKey);
        passcode[count] = CharToByte(pressedKey);
        count++;
      }
      
      else if (pressedKey == '#'){    //Reset passcode when # is pressed
        Sounds::KeyPressed();
        printf_P(PSTR("\nPasscode: "));
        clearPasscode();
        count = 0;
      }
      
      if(count == PASSCODELENGTH){    //Send passcode and proceed to wait for authorization            
        if(sendPasscode('B')){  
          state = 3;
          printf_P(PSTR("\n--Advancing to state 3 (Waiting for confirmation from Central Node)--\n"));        
        }
        clearPasscode();
        count = 0;
      }
     
      break;
      
      
    /////////////STATE 3//////////////
    ////Waiting for validation from Central Node  
    //////////////////////////////////
    case 3:
    
      if(network.available()){
        printf_P(PSTR("Received message --> "));
        RF24NetworkHeader header;
        bool accessGranted = false;
        
        network.peek(header);
        if(header.type == 'C'){ 
       
          network.read(header,&accessGranted,sizeof(bool));
      
          if(accessGranted){
            printf_P(PSTR("Access granted\n"));
            Sounds::AccessGranted();
            digitalWrite(REDLEDpin,LOW);
            digitalWrite(GREENLEDpin,HIGH);
            state = 1; 
            printf_P(PSTR("\n--Advancing to state 1 (Alarm deactivated)--\nPress * to activate alarm\n\n"));
          }
          else{
            printf_P(PSTR("Access denied"));
            Sounds::AccessDenied();
            state = 2; 
            printf_P(PSTR("\n--Advancing to state 2 (Alarm activated)--\nIntroduce the 6 character long passcode. Press # to clear.\nPasscode: "));
          }
        }
      }
      
      break; 
   
   
    /////////////STATE 4//////////////
    ////Waiting for new passcode
    //////////////////////////////////
    case 4:
    
      if(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()){ //Check if there is a new card and if it is readable
        Sounds::RFIDdetected();
        getRFIDpasscode();
        count = PASSCODELENGTH;
      }
    
      else if ((pressedKey >= '0') && (pressedKey <= '9')){  //Check if a number has been pressed and add it to passcode
        Sounds::KeyPressed();
        printf("%c ",pressedKey);
        passcode[count] = CharToByte(pressedKey);
        count++;
      }
      
      else if (pressedKey == '#'){    //Reset passcode when # is pressed
        Sounds::KeyPressed();
        printf_P(PSTR("\nNew passcode: "));
        clearPasscode();
        count = 0;
      }
      
      if(count == PASSCODELENGTH){    //Send passcode and proceed to wait for authorization            
        if(sendPasscode('D')){
          Sounds::SentNewPasscode();
          state = 1;
          Serial.println("\n--Advancing to state 1 (Alarm deactivated)--");
          digitalWrite(REDLEDpin,LOW);
          digitalWrite(GREENLEDpin,HIGH);
          
        }
        clearPasscode();
        count = 0;
      }
     
      break;
   
  
    //////////////////////////////////
    default:
      state = 1;
    
  }
    
  //Last minute modification to try solving bug in which RFID reader randomly stops working until the node is reset
  RestartRFIDReader();
}


///////////////////////////////////////////////////////////
////FUNCTIONS//////////////////////////////////////////////
///////////////////////////////////////////////////////////

//Transforms a character of a number (0-9) into a byte with that number's value:  '5' -> 00000101
byte CharToByte(char numberC)
{
  return(byte(numberC)-48);
}



//Sends a message to the Central Node to activate the alarm. Returns true if succesful.
bool sendActivationNotice()
{
  printf_P(PSTR("\nSending activation notice to Central Node.\n")); 
  
  bool sent = false;
  int retries = 0;
  RF24NetworkHeader header(0, 'A');  //(recipient,type)
  
  while((!sent)&&(retries<10)){
    Serial.print("Sending... ");
    if(retries>0) Sounds::Transmittion();
    sent = network.write(header,0,0);
    if(sent) printf_P(PSTR("sent.\n"));
    else printf_P(PSTR("failure.\n"));
    retries++;
  }
  
  if(!sent){
    Serial.println("Central Node does not respond. Make sure it is turned on and in range, and try again.\n");
    Sounds::CentralNodeUnresponsive();
  }
  return(sent);
}


//Fills the char array with spaces
void clearPasscode()
{
  for(int i=0; i<PASSCODELENGTH; i++){
    passcode[i] = 0; 
  }
}



/*
  Sends the passcode to the Central Node. Depending on the type it will be sent to request 
  verification or to be added to the database. 
  The passcode is stored in a global variable because network.write() does not work well with
  an array passed around as an argument.  
*/
bool sendPasscode(char type)
{

  if (type=='B') printf_P(PSTR("\nSending to Central Node for verification:\n"));
  else if (type=='D') printf_P(PSTR("\nSending to Central Node to be added to database:\n"));
  
  bool sent = false;
  int retries = 0;
  RF24NetworkHeader header(0, type);  //(recipient,type)
  
  while((!sent)&&(retries<10)){
    printf_P(PSTR("Sending... "));
    if(retries>0) Sounds::Transmittion();
    sent = network.write(header,&passcode,sizeof(passcode));
    if(sent) printf_P(PSTR("sent.\n"));
    else printf_P(PSTR("failure.\n"));
    retries++;
  }
  
  if(!sent){
    printf_P(PSTR("\nCentral Node does not respond. Make sure it is turned on and in range, and try again.\nPasscode:"));
    Sounds::CentralNodeUnresponsive();
  }
  return(sent);
}

  
  
/*
  Reads the PICC inside the RFID tag (its unique identification number) and stores it inside passcode[].
  PICCs are 4 bytes long. The remaining spaces inside passcode[] are filled with zeroes.
*/
void getRFIDpasscode()
{ 
  const int UIDlength = 4;  //Be careful: there are Mifare PICCs which have 4 or 7 bytes UID
  printf_P(PSTR("\nRFID tag: "));
  for (int i = 0; i < UIDlength; i++) { 
    passcode[i] = mfrc522.uid.uidByte[i];
    printf_P(PSTR("%u "),passcode[i]);
  }
  mfrc522.PICC_HaltA(); //Stop reading
  
  for (int i = UIDlength; i < PASSCODELENGTH; i++){
      passcode[i] = 0;
  } 
}



void RestartRFIDReader(){

  static long lastRestart = millis();
  
  if(millis()-lastRestart>3000)
  {
    mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
    mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); //Set Antenna Gain to Max. This will increase reading distance
    lastRestart = millis();
  }
}
