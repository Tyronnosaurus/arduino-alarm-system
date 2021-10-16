/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This code has been developed for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.

  This is the main sketch for the Central Node. Most of the code has been written inside custom libraries, so this file merely includes 
  these libraries, creates instances of their classes, starts them and updates them regularly.
*/


///////////////////////////////////////////////////////////
////INCLUDED LIBRARIES/////////////////////////////////////
///////////////////////////////////////////////////////////
#include <EEPROM.h>  //Used by Settings.h
#include "Settings.h"  //Functions to read and store user settings

#include <printf.h>  //Provides with function printf(). Very useful for debugging

#include <Wire.h>  //Used by RTClib.h
#include <RTClib.h>  //Interface for the Real Time Clock

#include <SPI.h>  //Used by RF24.h
#include <RF24.h>  //Used by RF24Network.h
#include <RF24Network.h>  //Used by Communications.h
#include "Communications.h"  //To communicate with other nodes in the network

#include "analogButton.h"  //Used by analogKeyboard.h
#include "analogKeyboard.h"  //Processes user input with the buttons

#include "StateMachine.h"  //Backend logic to navigate through menus

#include <LiquidCrystal.h>  //Used by Display.h
#include "Display.h"  //Displays screens on the LCD



///////////////////////////////////////////////////////////
////GLOBAL VARIABLES///////////////////////////////////////
///////////////////////////////////////////////////////////

////Declare instance for Real Time Clock control
RTC_DS1307 clock;

////Prepare wireless communications
Communications communications;

////Declare instance for the analog keyboard on pin A0
AnalogKeyboard analogKeyboard(A0);

////Declare instance for navigating menus
StateMachine stateMachine;

////Declare instance for LCD control
Display display;




///////////////////////////////////////////////////////////
///////SETUP///////////////////////////////////////////////
///////////////////////////////////////////////////////////
void setup()
{ 
  ////Setup Serial connection
  Serial.begin(57600);
  printf_begin();
  printf_P(PSTR("---------------------------------------------------------\n"));
  printf_P(PSTR("------------------------BASE NODE------------------------\n"));
  printf_P(PSTR("---------------------------------------------------------\n"));
  
  ////Setup the real time clock
  Wire.begin();
  clock.begin();
  //clock.adjust(DateTime(__DATE__, __TIME__));  //Adjusts the Real Time Clock to this computer's own clock.
                                                 //Use once, recomment and reupload, or the time will be reverted at each reset. 
  
  ////Setup wireless communications     
  communications.begin();
  
  ////Setup LCD screen
  display.Begin();

  printf_P(PSTR("\nSetup finished"));
}


///////////////////////////////////////////////////////////
///////LOOP////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void loop()
{     
  ////Interact with the rest of the network (check for new messages, send alerts...)
  communications.update();
  
  ////Check for user input with the pushbuttons
  analogKeyboard.update();
  
  ////Update the backend logic to navigate through the menus
  stateMachine.update(analogKeyboard,clock);
  
  ////Update the frontend to display information on the LCD screen
  display.update(stateMachine,analogKeyboard,clock);
}

