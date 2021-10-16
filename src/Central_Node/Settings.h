/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This library has been made for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.
  
  Its main function is allowing to read and write settings in permanent memory (EEPROM) so that they can 
  be recovered after the Arduino is powered down. It uses static methods, so it is not necessary to create
  an instance of Settings. Current settings implemented:
   - Alarm On/Off
   - Backlight mode
   - List of stored passcodes   
*/


#ifndef Sett_h
#define Sett_h


#include "Arduino.h"  //Needed to recognize 'byte'.
#include <EEPROM.h>


const int PASSCODELENGTH = 6;
const int MAXSTOREDPASSCODES = 10;

class Settings
{
  public:
    
    Settings();
    
    static void setAlarmState(bool);
    static bool isAlarmActivated();
    
    static bool isPasscodeInDatabase(byte passcode[PASSCODELENGTH]);
    static void addNewPasscode(byte passcode[PASSCODELENGTH]);
    static void deletePasscode(int index);
    static byte* getStoredPasscode(int index);
    
    static void setBacklightMode(byte mode);
    static byte getBacklightMode();
    
    static void RestoreFactorySettings();
    
    
    
  private:
  
    static bool comparePasscodes(byte passcode1[PASSCODELENGTH], byte passcode2[PASSCODELENGTH]);    
    static bool isPositionEmpty(int index);
    
    static byte* emptyPasscode();
    static void printStoredPasscodes();
    
    static void EEPROM_clear();
};



/*
EEPROM_writeAnything, EEPROM_readAnything
These functions interact with the Arduino's IC permanent memory (the EEPROM). The official library EEPROM.h 
only lets users read and write individual bytes. With these functions, any type of data can be stored.
Code provided by Arduino.cc @ http://playground.arduino.cc/Code/EEPROMWriteAnything

Why is it here and not in the cpp file? Because this library uses static methods, and you can't have 
static template methods in the cpp file, only in the headers file. More info here:
http://www.cplusplus.com/forum/general/128235/

template <class T> int EEPROM_writeAnything(int address, const T& parameter)
{
    const byte* p = (const byte*)(const void*)&parameter;
    unsigned int i;
    
    for (i = 0; i < sizeof(parameter); i++)
          EEPROM.write(address++, *p++);
    return i;
    
}

template <class T> int EEPROM_readAnything(int adress, T& parameter)
{
    byte* p = (byte*)(void*)&parameter;
    unsigned int i;
    for (i = 0; i < sizeof(parameter); i++)
          *p++ = EEPROM.read(adress++);    
    return i;
}*/

#endif

