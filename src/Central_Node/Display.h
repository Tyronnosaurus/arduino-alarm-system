/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This library has been made for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.
  
  Library for encapsulating functions that draw, on a 4x20 LCD screen, different displays used by the Central Node, 
  such as the main screen for when the alarm is idle, the menus with different settings the user can change, etc.
  This code is strictly frontend. No backend processing of the inner workings of the alarm is done here.
*/

#ifndef Display_h
#define Display_h


#include "Arduino.h"
#include "Settings.h"
#include <RTClib.h>
#include "StateMachine.h"
#include <LiquidCrystal.h>


class Display
{  
  public:
  
    Display();
    void Begin();
    void update(StateMachine, AnalogKeyboard, RTC_DS1307);
    void TurnBacklightOn();
    void TurnBacklightOff();
    
    
  private:
  
    LiquidCrystal lcd;
    bool backlightOn;
    
    void DisplayLoadingScreen();
    
    void prepareLCDCustomChars();
    void cleanScreen(StateMachine stateMachine);
    
    void displayMainScreen(RTC_DS1307 Clock);
    void displayTime(RTC_DS1307 Clock);
    void displayBigNumber(int num, int pos);
    void displayColon();
    void displaySpace();
    void displayDate(RTC_DS1307 Clock);
    
    void displayMenuRoot(StateMachine stateMachine);
    
    void displayTimeSelectionScreen(StateMachine stateMachine);
    void displayDateSelectionScreen(StateMachine stateMachine);
    void displayAddPasscodeScreen();
    void displayDeletePasscodeScreen(StateMachine stateMachine);
    void displayPasscode(int index);
    void displayChangeBacklightModeScreen(StateMachine stateMachine);
    void displayFactoryResetScreen(StateMachine stateMachine);
    
    void updateBacklight(AnalogKeyboard analogKeyboard);
};


#endif
