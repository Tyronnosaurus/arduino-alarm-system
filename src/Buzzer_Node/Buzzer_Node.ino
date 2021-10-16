/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This code has been developed for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.

  This is the main sketch for the Buzzer Node. The Buzzer Node's mission is to listen for any 
  message from the Central Node and emit a loud noise.
*/


///////////////////////////////////////////////////////////
////INCLUDED LIBRARIES/////////////////////////////////////
///////////////////////////////////////////////////////////
#include "Arduino.h"  //Needed by printf.h
#include <printf.h>

#include <SPI.h>  //Needed by RF24.h
#include <RF24.h>  //Needed by RF24Network.h
#include <RF24Network.h>



///////////////////////////////////////////////////////////
////CONSTANTS//////////////////////////////////////////////
///////////////////////////////////////////////////////////
const int CHANNEL = 100;
const uint16_t ADDRESS = 2;

const int BUZZpin = 4;



///////////////////////////////////////////////////////////
////GLOBAL VARIABLES///////////////////////////////////////
///////////////////////////////////////////////////////////

////Radio variables
RF24 radio(9,10);
RF24Network network(radio);



///////////////////////////////////////////////////////////
///////SETUP///////////////////////////////////////////////
///////////////////////////////////////////////////////////
void setup(void)
{
  //Print preamble 
  Serial.begin(57600);
  printf_begin();
  printf_P(PSTR("---------------------------------------------------------\n"));
  printf_P(PSTR("-----------------------PIEZO NODE------------------------\n"));
  printf_P(PSTR("---------------------------------------------------------\n"));
 
  //Bring up the RF network
  radio.begin();  
  network.begin(CHANNEL,ADDRESS);
  
  //Setup I/O pins
  pinMode(BUZZpin,OUTPUT);
  
  //Make a signal
  StartupSound();
  
  printf_P(PSTR("\nSetup finished\n\nChecking for incoming messages\n"));
}



///////////////////////////////////////////////////////////
///////LOOP////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void loop(void)
{  
  //Must be called regularly
  network.update();

  //Detect any new message
  if(network.available())
  {
    RF24NetworkHeader inHeader;  //A header object is like the envelope of the message. 
    network.peek(inHeader);

    if (inHeader.type == 'F'){
      printf_P(PSTR("\nReceived alert from Central Node.\n\r"));
      TriggerSound();    
    }
    
    network.read(inHeader,0,0); //Even though the message is empty, it is read to remove it from the queue
  }
  
  //Print a dot over Serial every 0.5 s to indicate that the node is running fine
  static long previousTime = 0;
  if (millis()-previousTime>500){ 
    printf_P(PSTR("."));
    previousTime = millis();
  }
}


///////////////////////////////////////////////////////////
///////FUNCTIONS///////////////////////////////////////////
///////////////////////////////////////////////////////////


//Make a signal when the board is powered on
void StartupSound()
{
  delay(500); //On the breadboard version, there is a capacitor which requires some time to charge at 
              //start, or else the buzzer will make a weak, ugly sound.
  
  for(int i=0;i<2;i++){
      digitalWrite(BUZZpin,HIGH);
      delay(300);
      digitalWrite(BUZZpin,LOW);
      delay(100);
    }
}



//Commands the buzzer to make a loud sound
void TriggerSound()
{
  for(int i=0;i<12;i++){
      digitalWrite(BUZZpin,HIGH);
      delay(300);
      digitalWrite(BUZZpin,LOW);
      delay(100);
    }
}

