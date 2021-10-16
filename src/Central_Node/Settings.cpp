#include "Settings.h"


//Constructor. Not needed, since all methods are static methods
Settings::Settings()
{  

}



/*
Sets alarm activated or deactivated. 
If activated is true, activates the alarm. If false, deactivates the alarm.
*/
void Settings::setAlarmState(bool activated)
{
  EEPROM.write(0, activated);
}



//Returns true if alarm is activated
bool Settings::isAlarmActivated()
{
  return(EEPROM.read(0));
}



//Compares a given passcode with all stored passcodes in database. If a coincidence is found, returns true
bool Settings::isPasscodeInDatabase(byte passcode[PASSCODELENGTH])
{
  bool found = false;
  int i=0;
  while((i<MAXSTOREDPASSCODES) && (!found)){
    found = comparePasscodes(passcode,getStoredPasscode(i));
    i++;
  }
  return(found);
}



//Checks if the given passcode is the same as the one stored in given index inside the database
bool Settings::comparePasscodes(byte passcode1[PASSCODELENGTH], byte passcode2[PASSCODELENGTH])
{    
  bool foundDifference = false;
  int i = 0;
  while((i < PASSCODELENGTH) && (!foundDifference)){
    foundDifference = (passcode1[i] != passcode2[i]);
    i++;
  }
  return (!foundDifference);  
}



//Returns the passcode stored in database position 'index'
byte* Settings::getStoredPasscode(int index)
{  
  static byte storedPasscode[PASSCODELENGTH];
  int address = 100 + (index*PASSCODELENGTH);
  EEPROM.get(address,storedPasscode);
  return(storedPasscode);
}


//Attempts adding the given passcode to the database inside the EEPROM
void Settings::addNewPasscode(byte newPasscode[PASSCODELENGTH])
{  
  if(comparePasscodes(newPasscode,emptyPasscode())){  //Do not add empty passcodes
    printf("\nInvalid passcode. It cannot be full of 0s.\n");
    return;
  } 
  if(isPasscodeInDatabase(newPasscode)){  //Do not add repeated passcodes
    printf("\nInvalid passcode. Already in database.\n");
    return;
  }
  
  bool added = false;  
  
  int index = 0;
  while((index < MAXSTOREDPASSCODES) && (!added)){
    if (isPositionEmpty(index)){
      int EEaddress = 100 + index*PASSCODELENGTH;
      for(int i=0; i<PASSCODELENGTH; i++) EEPROM.write(EEaddress+i,newPasscode[i]);
      added = true;
    }
    index++;
  }
  if(added) printf("\nSuccessfully added to database in index %u.\n", index-1);
  else printf("\nDatabase full. Could not add new passcode.\n");
  
  printf("\nCurrent database :\n");
  printStoredPasscodes();
}



/*
Checks whether the position with given index in the passcodes database is full or empty.
Positions filled with 0s are considered empty.
*/
bool Settings::isPositionEmpty(int index)
{   
  bool isEmpty = comparePasscodes(emptyPasscode(), getStoredPasscode(index));
  return(isEmpty);  
}



//Generates a passcode filled with 0s, which is considered empty
byte* Settings::emptyPasscode()
{
  static byte emptyPasscode[PASSCODELENGTH];
  for (int i=0; i<PASSCODELENGTH; i++) emptyPasscode[i] = 0;
  return(emptyPasscode); 
}



//Prints a list of stored passcodes in Serial console. Used mostly for debugging
void Settings::printStoredPasscodes()
{  
  printf_P(PSTR("Index    Passcode\n"));
  for(int i=0; i<MAXSTOREDPASSCODES; i++){
    printf_P(PSTR("  %u      "),i);
    for(int j=0; j<PASSCODELENGTH; j++){
      printf_P(PSTR("%u "),EEPROM.read(100 + i*PASSCODELENGTH + j));  
    }
    printf_P(PSTR("\n"));
  }
}



//Deletes passcode with given index from database
void Settings::deletePasscode(int index)
{  
  int EEaddress = 100 + index*PASSCODELENGTH;
  for(int i=0; i<PASSCODELENGTH; i++) EEPROM.write(EEaddress+i,0);
}




//Changes how the LCD backlight acts:
//  - Mode 0: Always on
//  - Mode 1: Only on button presses
//  - Mode 2: Always off
void Settings::setBacklightMode(byte mode)
{
    EEPROM.write(1,mode);
}

//Returns backlight mode
byte Settings::getBacklightMode()
{
  return(EEPROM.read(1)); 
}




//Reset settings to default values (used for testing and debugging, for now)
void Settings::RestoreFactorySettings()
{
   //Alarm comes deactivated from factory (obviously)
   setAlarmState(false);
   
   //Backlight mode: default mode is 'Always On'
   setBacklightMode(0);
   
   //Set factory included passcodes to whatever you want
   byte passcodes[PASSCODELENGTH*MAXSTOREDPASSCODES] = {1,2,3,4,5,6,  195,136,150,198,0,0,  0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,0,   0,0,0,0,0,0,  0,0,0,0,0,0,   0,0,0,0,0,0,  0,0,0,0,0,0,   0,0,0,0,0,0};
   EEPROM.put(100, passcodes);
}


/*
Used only for debugging
Write a 0 to all 1024 bytes of the EEPROM. Do not abuse this function. Every bit in 
the EEPROM has a finite number of writes. Try to clear only those bits that actually store data.
*/
void Settings::EEPROM_clear(){
  for (int i = 0; i < 1024; i++) EEPROM.write(i, 0);
}
