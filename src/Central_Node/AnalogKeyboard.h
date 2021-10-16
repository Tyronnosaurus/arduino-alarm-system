/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This library has been made for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.

  This library handles a panel of many pushbuttons connected in an analog configuration. This makes it possible to
  work with many pushbuttons without having an instance for each one in the main sketch (they are hidden here).
  It also provides methods to easily check if any key has been pressed.
*/



#ifndef anaKey_h
#define anaKey_h


#include "AnalogButton.h"


class AnalogKeyboard
{
  public:
    AnalogKeyboard(int myPin);
    
    AnalogButton BTNSelect;
    AnalogButton BTNUp; 
    AnalogButton BTNRight;
    AnalogButton BTNDown;
    AnalogButton BTNLeft;

    void update();
    bool wasAnyArrowPressed();
    bool wasAnyButtonPressed();	
  
  
  private:
    int pin;
  
};


#endif
