#include "StateMachine.h"


//Constructor
StateMachine::StateMachine()
{
  ////Setup navigation variables
  state = 0;  //Start at main menu
  cursorPosition = 0;
}


/*
Navigate through the state machine
In each state, the user input is analysed. For example, if the user pressed Down while in a state representing a 
list of items, the cursor would be moved one position down. Or if the user selects a new menu, the state  of the 
FSM would change to enter that menu.
This class deals with the backend logic behind the state machine. Nothing is drawn on the LCD by this class
*/
void StateMachine::update(AnalogKeyboard analogKeyboard, RTC_DS1307 clock)
{
  
  switch(state){
    
    /////////////////////////////////////////////////
    ////State 0
    ////Main Screen
    /////////////////////////////////////////////////
    case 0:
      if (analogKeyboard.wasAnyArrowPressed()) state = 1;  //Press an arrow to access the Root Menu. The Select button does
                                                  //not access the Root Menu since it is only used to turn on the 
                                                  //backlight while in the Main Screen  
      break;
    
    
    /////////////////////////////////////////////////
    ////State 1 - Root Menu
    ////Contains a list of submenus (Change time, change date, delete passcodes...).
    ////The user can move a cursor through the list to select an entry.
    /////////////////////////////////////////////////
    case 1:
      
      ////Pressed LEFT: go back to main screen
      if (analogKeyboard.BTNLeft.wasPressed()){
        state = 0;
        cursorPosition = 0;
      }
      
      ////Pressed UP or DOWN: move the cursor
      else if (analogKeyboard.BTNDown.wasPressed() && cursorPosition<5) cursorPosition++;
      else if (analogKeyboard.BTNUp.wasPressed() && cursorPosition>0) cursorPosition--;
    
      ////Pressed SELECT or RIGHT: access selected entry in the list
      else if (analogKeyboard.BTNSelect.wasPressed() || analogKeyboard.BTNRight.wasPressed()){   
        
        if (cursorPosition == 0){  //Selected entry: Change Time
          state = 2;
          cursorPosition=0;
          DateTime now = clock.now();
          tempHour = now.hour();
          tempMinute = now.minute();  
        }
        if (cursorPosition == 1){  //Selected entry: Change Date
          state = 3;
          DateTime now = clock.now();
          tempDay = now.day();
          tempMonth = now.month();
          tempYear = now.year();
        }
        if (cursorPosition == 2){  //Selected entry: Add passcode/RFIDtag 
          state = 4;  
        }
        if (cursorPosition == 3){  //Selected entry: Delete passcode/RFIDtag 
          state = 5;  
        }
        if (cursorPosition == 4){  //Selected entry: Change backlight mode 
          state = 6;  
        }
        if (cursorPosition == 5){  //Selected entry: Restore factory settings 
          state = 7;  
        }
             
        cursorPosition=0;
        
      }
      
      break;
    
    
    
    /////////////////////////////////////////////////
    ////State 2 - Change time
    ////Submenu to change hour and minutes
    /////////////////////////////////////////////////
    case 2:
      if (analogKeyboard.BTNRight.wasPressed() && cursorPosition<3){  //Move cursor right
        cursorPosition++; 
      }
      else if (analogKeyboard.BTNLeft.wasPressed() && cursorPosition>0){  //Move cursor left
        cursorPosition--; 
      }
      
      else if (analogKeyboard.BTNUp.wasPressed()){  //Increase selected value
        if (cursorPosition==0){
          if (tempHour<23)tempHour++;
          else tempHour=0;
        }
        else if (cursorPosition==1){
          if (tempMinute<59)tempMinute++;
          else tempMinute=0;
        }
      }
      else if (analogKeyboard.BTNDown.wasPressed()){  //Decrease selected value
        if (cursorPosition==0){
          if (tempHour>0)tempHour--;
          else tempHour=23;
        }
        else if (cursorPosition==1){
          if (tempMinute>0)tempMinute--;
          else tempMinute=59;
        }
      }
      else if (analogKeyboard.BTNSelect.wasPressed()){
        if (cursorPosition == 2){                //Selected OK: Save and exit
          DateTime now = clock.now();
          DateTime dateTime(now.year(), now.month(), now.day(), tempHour, tempMinute, 0);
          clock.adjust(dateTime);
          state = 0;
          cursorPosition=0;
        }
        else if (cursorPosition==3){             //Selected BACK: Exit
          state = 0;
          cursorPosition=0;
        }       
      }
    
      break;



    /////////////////////////////////////////////////
    ////State 3 - Change date
    ////Submenu to change day, month and year
    /////////////////////////////////////////////////
    case 3:
      if (analogKeyboard.BTNRight.wasPressed() && cursorPosition<4){  //Move cursor right
        cursorPosition++; 
      }
      else if (analogKeyboard.BTNLeft.wasPressed() && cursorPosition>0){  //Move cursor left
        cursorPosition--; 
      }
      
      else if (analogKeyboard.BTNUp.wasPressed()){  //Increase selected value
        if (cursorPosition==0){
          if (tempDay<31)tempDay++;
          else tempDay=1;
        }
        else if (cursorPosition==1){
          if (tempMonth<12)tempMonth++;
          else tempMonth=1;
        }
        else if (cursorPosition==2){
          if (tempYear<3000)tempYear++;
        }
      }
      else if (analogKeyboard.BTNDown.wasPressed()){  //Decrease selected value
        if (cursorPosition==0){
          if (tempDay>1)tempDay--;
          else tempDay=31;
        }
        else if (cursorPosition==1){
          if (tempMonth>1)tempMonth--;
          else tempMonth=12;
        }
        else if (cursorPosition==2){
          if (tempYear>2000)tempYear--;
        }
      }
      else if (analogKeyboard.BTNSelect.wasPressed()){
        if (cursorPosition == 3){                //Selected OK: Save and exit
          DateTime now = clock.now();
          DateTime dateTime(tempYear, tempMonth, tempDay, now.hour(), now.minute(), now.second());
          clock.adjust(dateTime);
          state = 0;
          cursorPosition=0;
        }
        else if (cursorPosition==4){             //Selected BACK: Exit
          state = 0;
          cursorPosition=0;
        }       
      }
      
      break;


    /////////////////////////////////////////////////
    ////State 4 - Add passcode/RFID tag
    ////This is mostly done from the Key Tray Node, so there's not much to do here.
    ////Just display the instructions onscreen.
    /////////////////////////////////////////////////
    case 4:
    
      if (analogKeyboard.wasAnyButtonPressed()) state = 0;
    
    break;
    
    

    /////////////////////////////////////////////////
    ////State 5 - Delete passcode/RFID tag
    ////Submenu to delete one of the passcodes in memory
    /////////////////////////////////////////////////
    case 5:
      if (analogKeyboard.BTNUp.wasPressed() && cursorPosition>0){  //Move cursor up
        cursorPosition--;
      }
      if (analogKeyboard.BTNDown.wasPressed() && cursorPosition<MAXSTOREDPASSCODES){  //Move cursor down 
        cursorPosition++;
      }
      if (analogKeyboard.BTNLeft.wasPressed()){
        cursorPosition = 0;
        state = 0;
        
      }
      if (analogKeyboard.BTNSelect.wasPressed()){
        Settings::deletePasscode(cursorPosition);
        cursorPosition = 0;
        state = 0;
      }
      
    break;
    
    
    /////////////////////////////////////////////////
    ////State 6 - Change Backlight Mode
    ////Submenu to change how the LCD backlight acts (always on or temporally on)
    /////////////////////////////////////////////////
    case 6:
      if (analogKeyboard.BTNUp.wasPressed() && cursorPosition>0) cursorPosition--;
      if (analogKeyboard.BTNDown.wasPressed() && cursorPosition<2) cursorPosition++;
      
      if (analogKeyboard.BTNLeft.wasPressed()){  //Exit
        cursorPosition = 0;
        state = 0;
      }
      
      if (analogKeyboard.BTNSelect.wasPressed()){
        if (cursorPosition == 0) Settings::setBacklightMode(0);       //Mode 'Always on'
        else if (cursorPosition == 1) Settings::setBacklightMode(1);  //Mode 'On button press'
        else if (cursorPosition == 2) Settings::setBacklightMode(2);  //Mode 'Always off'     
        
        cursorPosition = 0;
        state = 0;
      }

      break;
      
      
    /////////////////////////////////////////////////
    ////State 7 - Reset to factory settings
    ////Lets the user restore all settings to default (alarm state to deactivated, 
    ////backlight mode to always on, and all passcode deleted). This will not reconfigure date and time.
    /////////////////////////////////////////////////
    case 7:
    
    if (analogKeyboard.BTNRight.wasPressed() && cursorPosition<1){  //Move cursor right
        cursorPosition++; 
      }
      else if (analogKeyboard.BTNLeft.wasPressed() && cursorPosition>0){  //Move cursor left
        cursorPosition--; 
      }
      
      else if (analogKeyboard.BTNSelect.wasPressed()){
        if (cursorPosition == 0){                //Selected NO: Exit
          state = 0;
          cursorPosition=0;
        }
        else if (cursorPosition==1){             //Selected YES: Restore settings and exit
          Settings::RestoreFactorySettings();
          state = 0;
          cursorPosition=0;
        }       
      }
    
      break;
    
    
    
    
  }

}


//Returns the current state of the FSM
int StateMachine::GetState()
{
  return(state);
}



//Returns the current cursor position
int StateMachine::GetCursorPosition()
{
  return(cursorPosition);  
}
