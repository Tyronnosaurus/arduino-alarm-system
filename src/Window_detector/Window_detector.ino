/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This code has been developed for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.

  This is the main sketch for the Window Detector Node. It has a momentary switch with a long arm intended to be 
  placed close to the edge of a door or window. When the door or window is opened, it closes the switch for a moment 
  and the node sends an alert message to the Central Node.
*/


///////////////////////////////////////////////////////////
////INCLUDED LIBRARIES/////////////////////////////////////
///////////////////////////////////////////////////////////
#include <Arduino.h>  //Needed by printf.h
#include <printf.h>  

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

#include <avr/sleep.h>
#include <EnableInterrupt.h>

///////////////////////////////////////////////////////////
////CONSTANTS//////////////////////////////////////////////
///////////////////////////////////////////////////////////
const int CHANNEL = 100;
const uint16_t ADDRESS = 4;  //Window Nodes all have address 4

const int SWITCHpin = 2;  //Used as an Interrupt pin
const int LEDpin = 4;
                                   

///////////////////////////////////////////////////////////
////GLOBAL VARIABLES///////////////////////////////////////
///////////////////////////////////////////////////////////
RF24 radio(9,10);
RF24Network network(radio);


///////////////////////////////////////////////////////////
///////SETUP///////////////////////////////////////////////
///////////////////////////////////////////////////////////
void setup()
{
  ////SERIAL
  Serial.begin(57600);
  printf_begin();
  
  delay(100); //Give time for the filter capacitor to charge
  
  ////Prepare I/O pins
  pinMode(LEDpin,OUTPUT);
  
  ////Bring up the RF network
  radio.begin();
  network.begin(CHANNEL,ADDRESS);  //Also prints radio properties
  
  ActivationSignal();
}



///////////////////////////////////////////////////////////
///////LOOP////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void loop()
{
  ATmega328pGoToSleep();  //Go to sleep until woken by an external interrupt caused by the switch.
  
  delay(300);
  
  SendAlert();
  LEDsignal();
}



///////////////////////////////////////////////////////////
////FUNCTIONS//////////////////////////////////////////////
///////////////////////////////////////////////////////////

//Makes a visual cue when the node is powered on.
void ActivationSignal()
{
  for (int i=0;i<5;i++)
  {
    delay(200);
    digitalWrite(LEDpin,HIGH);
    delay(200);
    digitalWrite(LEDpin,LOW);
  }
  
  printf_P(PSTR("Setup finished\n"));
}



//Send an alert message to Central Node.
void SendAlert()
{
  RF24NetworkHeader header(0, 'G'); //(to node, type)
  
  printf_P(PSTR("---------------------------------\n\r"));
  printf_P(PSTR("APP Sending alert to Central Node...\n\r"));
  if(network.write(header,0,0)) printf_P(PSTR("Sent ok\n"));
}



//Makes a visual cue with the LED to indicate that the node has been triggered.
void LEDsignal()
{
  digitalWrite(LEDpin,HIGH);
  delay(5000);
  digitalWrite(LEDpin,LOW);
}



//The following block puts the IC to sleep until it is woken up by an Interrupt.
void ATmega328pGoToSleep()
{ 
  printf_P(PSTR("Going to sleep\n"));
  delay(100);
  
  //Disable ADC
  ADCSRA = 0;
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  //Do not interrupt before we go to sleep, or the
  //ISR will detach interrupts and we won't wake.
  noInterrupts();
  
  //Attach interrupt to a pin  
  enableInterrupt(SWITCHpin, Wake, CHANGE);
  
  //Turn off brown-out enable in software
  //BODS must be set to one and BODSE must be set to zero within four clock cycles
  MCUCR = bit(BODS) | bit(BODSE);
  //The BODS bit is automatically cleared after three clock cycles
  MCUCR = bit(BODS);
  
  // We are guaranteed that the sleep_cpu call will be done as the processor executes 
  //the next instruction after interrupts are turned on.
  interrupts();  //One cycle
  sleep_cpu();   //One cycle
}


/*
ISR (Interrupt service routine). Run when waking up from sleep
Delays, timers and serial don't work until interrupts are detached
*/
void Wake()  
{  
  //Cancel sleep as a precaution
  sleep_disable();
  //Detach interrupts to prevent unwanted wakeups in the future
  disableInterrupt(SWITCHpin);
  
  delay(100);
  printf_P(PSTR("Wake up\n"));
  delay(100);
}
