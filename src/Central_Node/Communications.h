/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This library has been made for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.

  This library addresses how to handle incoming messages through the wireless nRF24l01+ module. Depending 
  on the type of the incoming message, different actions will be taken:
  - Send an alert to the Buzzer Node if the alarm is activated.
  - Modifying a setting.
  - etc.
*/


#ifndef Communications_h
#define Communications_h


#include "Settings.h"
#include <RF24.h>
#include <RF24Network.h>



class Communications
{
  public:
    Communications();
    void begin();
    void update();

    
  private:
    RF24 radio;
    RF24Network network; 
};


#endif
