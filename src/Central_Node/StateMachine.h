/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This library has been made for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.
  
  Library that encapsulates methods to control a Finite State Machines that governs the navigation between menus in the 
  Central Node. For example, if the FSM is at state 3 we are in the Change Time Submenu, and any user input will be processed 
  accordingly to increase minutes, save changes, etc.

  Not that this only deals with the backend logic of the FSM. The LCD display is performed by the Display class after 
  consulting the current state of the FSM.
*/





#ifndef StateMach_h
#define StateMach_h

#include "analogKeyboard.h"
#include "Settings.h"
#include <RTClib.h>


class StateMachine{

  public:
  
    StateMachine();
    void update(AnalogKeyboard analogKeyboard, RTC_DS1307 clock);
    int GetState();
    int GetCursorPosition();
    
    //Temporal values used while changing time and date (they appear on the screen but aren't really saved until the user says so)
    uint8_t tempMinute;
    uint8_t tempHour;
    uint8_t tempDay;
    uint8_t tempMonth;
    uint16_t tempYear;
    
    
  private:

    ////State machine navigation variables
    int state;  //The current state of the state machine
    int cursorPosition;  //Some states are lists of entries. This keeps track of which one is seleted
  

};




#endif
