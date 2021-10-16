/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This code has been developed for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.

  This library is used to generate sounds and short melodies for the piezo in the Key Tray Node. They are used to
  give the user feedback whenever he intracts with the device.
*/


#ifndef sound_h
#define sound_h

#include "Arduino.h"
#include "Pitches.h"

#define PIEZOpin 10

class Sounds{
  
  public:
    
    Sounds();
    static void KeyPressed();
    static void RFIDdetected();
    static void AccessGranted();
    static void AccessDenied();
    static void AlarmActivated();
    static void AddingNewPasscode();
    static void SentNewPasscode();
    static void Transmittion();
    static void CentralNodeUnresponsive();
    
  private:
  
    static void playMelody(int melody[], int noteDurations[], int length);
  
  
};

#endif
