/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This library has been made for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.

  This library handles pushbuttons connected in an analog configuration (pressing them connects a voltage divider).
  Its backend is a simple finite state machine that tracks presses and releases. It takes care of debouncing.
*/

#ifndef anaButt_h
#define anaButt_h

#include "Arduino.h"



class AnalogButton
{
  public:
    AnalogButton(int myMin, int myMax, int myDebounceTime);
    void update(int analogVal);
    bool wasPressed();
    bool wasReleased();
	
	
  private:
	
    int min;
    int max;
    int debounceTime;

    byte previousState;
    byte state;
    long auxTime;  //Holds the exact time the button was last pressed or released
	
};


#endif
