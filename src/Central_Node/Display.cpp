#include "Display.h"


#define BACKLIGHTpin 13
#define BACKLIGHTTIME 10  //Seconds the backlight remains on




//Class constructor. The LiquidCrystal member is initialized here, in the initializer list
Display::Display() : lcd(8,7,6,5,4,3) //Initializer
{
  
}



/*
Prepares the lcd screen, creates special characters to draw big numbers, 
and turns on the backlight.
*/
void Display::Begin()
{
  prepareLCDCustomChars();
  lcd.begin(20,4);
 
  pinMode(BACKLIGHTpin,OUTPUT);
  TurnBacklightOn();
  
  DisplayLoadingScreen();  //Not really necessary, only provides visual cue of startup
}



/*
Creates special characters to draw big numbers
ASCII art by mpilchfamily @ Instructables.com - http://www.instructables.com/id/Custom-Large-Font-For-16x2-lcds/
*/
void Display::prepareLCDCustomChars()
{
  //Create bitmaps (also called glyphs) for drawing big numbers
  byte LT[8] = {B00111, B01111, B11111, B11111, B11111, B11111, B11111, B11111};
  byte UB[8] = {B11111, B11111, B11111, B00000, B00000, B00000, B00000, B00000};
  byte RT[8] = {B11100, B11110, B11111, B11111, B11111, B11111, B11111, B11111};
  byte LL[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B01111, B00111};
  byte LB[8] = {B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111};
  byte LR[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11110, B11100};
  byte UMB[8] = {B11111, B11111, B11111, B00000, B00000, B00000, B11111, B11111};
  byte LMB[8] = {B11111, B00000, B00000, B00000, B00000, B11111, B11111, B11111};

  //Register the bitmaps as characters for the lcd (each with a given index)
  lcd.createChar(1,UB);
  lcd.createChar(2,RT);
  lcd.createChar(3,LL);
  lcd.createChar(4,LB);
  lcd.createChar(5,LR);
  lcd.createChar(6,UMB);
  lcd.createChar(7,LMB);
  lcd.createChar(8,LT);
}



/*
Display a screen with a loading bar for when the program starts. 
The bar currently holds no purpose, since it runs synchronously (blocks execution of others tasks) and just wastes time.
However it gives the user a useful visual cue.
*/
void Display::DisplayLoadingScreen()
{  
  lcd.clear();
  lcd.print("       LOADING");
  
  lcd.setCursor(0,1);
  lcd.print("--------------------");
  lcd.setCursor(0,3);
  lcd.print("--------------------");
  
  lcd.setCursor(0,2);
  for(int i=0;i<20;i++){
    lcd.print("|");
    delay(50);  
  }
  delay(400);
  lcd.clear();
}



/*
  Public function to display current screen. Just call this one, give it its parameters and it will work out 
  which subscreen to display, where to draw the cursor, etc.
  StateMachine.h contains the backend logic behind the finite state machine used to navigate the different menus. 
  Display.h is the just the frontend. It checks the current state of the FSM and draws the corresponding screen.
*/
void Display::update(StateMachine stateMachine, AnalogKeyboard analogKeyboard, RTC_DS1307 Clock)
{  
  updateBacklight(analogKeyboard); //Do we need to turn on or off the LCD backlight?
  cleanScreen(stateMachine);
  
  switch(stateMachine.GetState()){
    
    case 0:
      displayMainScreen(Clock); 
      break;
         
    case 1:
       displayMenuRoot(stateMachine);
       break;
  
    case 2:
      displayTimeSelectionScreen(stateMachine);
      break;
  
    case 3:
      displayDateSelectionScreen(stateMachine);
      break;
      
    case 4:
      displayAddPasscodeScreen();
      break;
      
    case 5:
      displayDeletePasscodeScreen(stateMachine);
      break;
     
    case 6:
      displayChangeBacklightModeScreen(stateMachine);
      break;
      
    case 7:
      displayFactoryResetScreen(stateMachine);
  }
}


/*
Clears the screen. The screen can't be cleared continuously since it makes the screen flicker. Instead, 
it is cleaned whenever the user accedes to a new subbmenu.
*/
void Display::cleanScreen(StateMachine stateMachine)
{
  static int previousState = stateMachine.GetState();
  static int previousCursorPosition = stateMachine.GetCursorPosition();
  
  if ((previousState != stateMachine.GetState()) || (previousCursorPosition != stateMachine.GetCursorPosition())) lcd.clear();
  
  previousState = stateMachine.GetState();
  previousCursorPosition = stateMachine.GetCursorPosition();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The following methods are used to display the main screen (with the time, date, alarm state...)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Display the main screen with clock, date, alarm state...
void Display::displayMainScreen(RTC_DS1307 Clock)
{ 
  displayTime(Clock);
  displayDate(Clock);
  
  //Display alarm state
  lcd.setCursor(15,1);
  lcd.print("ALARM");
  lcd.setCursor(16,2);
  if(Settings::isAlarmActivated()) lcd.print("ON ");
  else lcd.print("OFF");
}



//Display the clock portion of the main screen
void Display::displayTime(RTC_DS1307 Clock)
{
  DateTime now = Clock.now();
 
  displayBigNumber(now.hour() / 10, 0);  
  displayBigNumber(now.hour() % 10, 1); 
  if(now.second() % 2) displayColon();  //Only prints the colon on odd seconds
  else displaySpace();
  displayBigNumber(now.minute() / 10, 2);
  displayBigNumber(now.minute() % 10, 3);
  
  lcd.setCursor(11,2);
  lcd.print(now.second()/10);
  lcd.print(now.second()%10);
}



/*
Displays the number num in a 3x2 char custom font. In addition, pos indicates the position (0:hour decimal, 1:hour unit, 2:minute decimal, 3:minute unit)
*/
void Display::displayBigNumber(int num, int pos)
{
  //Calculate the position on the lcd
  int x = pos*3;
  if (x>=6) x++;
   
  //Draw the whole number with previously created bitmaps 
  switch (num){
    case 0:
      lcd.setCursor(x, 0); lcd.write(8); lcd.write(1); lcd.write(2); lcd.setCursor(x, 1); lcd.write(3); lcd.write(4); lcd.write(5);
      break;
      
    case 1:
      lcd.setCursor(x,0); lcd.write(1); lcd.write(2); lcd.write(' '); lcd.setCursor(x,1); lcd.write(' '); lcd.write(255); lcd.write(' ');
      break;
      
    case 2:
      lcd.setCursor(x,0); lcd.write(6); lcd.write(6); lcd.write(2); lcd.setCursor(x, 1); lcd.write(3); lcd.write(7); lcd.write(7);
      break;
      
    case 3:
      lcd.setCursor(x,0); lcd.write(6); lcd.write(6); lcd.write(2); lcd.setCursor(x, 1); lcd.write(7); lcd.write(7); lcd.write(5); 
      break;
      
    case 4:
      lcd.setCursor(x,0); lcd.write(3); lcd.write(4); lcd.write(2); lcd.setCursor(x, 1); lcd.write(' '); lcd.write(' '); lcd.write(255);
      break;
      
    case 5:
      lcd.setCursor(x,0); lcd.write(255); lcd.write(6); lcd.write(6); lcd.setCursor(x, 1); lcd.write(7); lcd.write(7); lcd.write(5);
      break;
      
    case 6:
      lcd.setCursor(x,0); lcd.write(8); lcd.write(6); lcd.write(6); lcd.setCursor(x, 1); lcd.write(3); lcd.write(7); lcd.write(5);
      break;
      
    case 7:
      lcd.setCursor(x,0); lcd.write(1); lcd.write(1); lcd.write(2); lcd.setCursor(x, 1); lcd.write(' '); lcd.write(8); lcd.write(' ');
      break;
      
    case 8:
      lcd.setCursor(x,0); lcd.write(8); lcd.write(6); lcd.write(2); lcd.setCursor(x, 1); lcd.write(3); lcd.write(7); lcd.write(5);
      break;
      
    case 9:
      lcd.setCursor(x,0); lcd.write(8); lcd.write(6); lcd.write(2); lcd.setCursor(x, 1); lcd.write(' '); lcd.write(' '); lcd.write(255);
      break;
      
    default:
      break;   
  }    
}



//Display the colon between hours and minutes
void Display::displayColon()
{
  lcd.setCursor(6,0);
  lcd.write(B10100101);
  lcd.setCursor(6,1);
  lcd.write(B10100101);
}



//Display a blank space between hours and minutes
void Display::displaySpace()
{
  lcd.setCursor(6,0);
  lcd.write(' ');
  lcd.setCursor(6,1);
  lcd.write(' ');
}



//Display the full date on the bottom row (Example: 12 May'15)
void Display::displayDate(RTC_DS1307 clock)
{
  DateTime now = clock.now();
  
  lcd.setCursor(0,4);
  if(now.day()<10) lcd.print('0');  //Pad with a zero if necessary
  lcd.print(now.day());
  
  const char* month[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  lcd.print(' ');
  lcd.print(month[now.month()-1]);
  
  lcd.print(" '");
  if(now.year()%100<10) lcd.print('0');
  lcd.print(now.year()%100);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The following methods are used to display screens where the user can change different settings
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Display root menu with the list of possible settings to change 
void Display::displayMenuRoot(StateMachine stateMachine)
{  
  const char* MenuItems[] = {"Change Time", "Change Date", "Add passcode", "Delete passcode", "Change LCD light", "Reset settings"};
  
  //Print title on first line
  lcd.setCursor(0,0);
  lcd.print("______Main Menu_____");
  
  //Print three entries
  if(stateMachine.GetCursorPosition()>0){
    lcd.setCursor(1,1);
    lcd.print(MenuItems[stateMachine.GetCursorPosition()-1]);
    lcd.setCursor(19,1);
    lcd.write(94);  //Arrow up
  }
  
  lcd.setCursor(0,2);
  lcd.write(161);  //Display the cursor
  lcd.print(MenuItems[stateMachine.GetCursorPosition()]);
 
  if(stateMachine.GetCursorPosition()<5){
    lcd.setCursor(1,3);
    lcd.print(MenuItems[stateMachine.GetCursorPosition()+1]);
    lcd.setCursor(19,3);
    lcd.print('v');  //Arrow down
  }
}



//Displays the screen to change time
void Display::displayTimeSelectionScreen(StateMachine stateMachine)
{  
  lcd.setCursor(0,0);
  lcd.print("_____Change time____");
  lcd.setCursor(1,2);
  if(stateMachine.tempHour<10) lcd.print('0');
  lcd.print(stateMachine.tempHour);
  lcd.print(" : ");
  if(stateMachine.tempMinute<10) lcd.print('0');
  lcd.print(stateMachine.tempMinute);
  lcd.print("    OK BACK");

  if(stateMachine.GetCursorPosition()==0) lcd.setCursor(1,3);
  else if(stateMachine.GetCursorPosition()==1) lcd.setCursor(6,3);
  else if(stateMachine.GetCursorPosition()==2) lcd.setCursor(12,3);
  else if(stateMachine.GetCursorPosition()==3) lcd.setCursor(16,3);
  lcd.print("--");
}



//Displays the screen to change date
void Display::displayDateSelectionScreen(StateMachine stateMachine)
{  
  lcd.setCursor(0,0);
  lcd.print("_____Change date____");
  lcd.setCursor(1,2);
  if(stateMachine.tempDay<10) lcd.print('0');
  lcd.print(stateMachine.tempDay);
  lcd.print("/");
  if(stateMachine.tempMonth<10) lcd.print('0');
  lcd.print(stateMachine.tempMonth);
  lcd.print("/");
  if(stateMachine.tempYear<10) lcd.print('0');
  lcd.print(stateMachine.tempYear);
  
  lcd.print(" OK BACK");

  if(stateMachine.GetCursorPosition()==0) lcd.setCursor(1,3);
  else if(stateMachine.GetCursorPosition()==1) lcd.setCursor(4,3);
  else if(stateMachine.GetCursorPosition()==2) lcd.setCursor(8,3);
  else if(stateMachine.GetCursorPosition()==3) lcd.setCursor(12,3);
  else if(stateMachine.GetCursorPosition()==4) lcd.setCursor(16,3);
  lcd.print("--");
}



//Display screen with instructions to add a new passcode or RFID tag
void Display::displayAddPasscodeScreen()
{
  lcd.setCursor(0,0); lcd.print("With alarm off press");
  lcd.setCursor(0,1); lcd.print(" # on Key Tray Node");
  lcd.setCursor(0,2); lcd.print("and enter new pass-");
  lcd.setCursor(0,3); lcd.print("code/RFID tag."); 
}



//Display a menu to let the user select one of the stored passcodes to be deleted 
void Display::displayDeletePasscodeScreen(StateMachine stateMachine)
{
  //Print title on first line
  lcd.setCursor(0,0);
  lcd.print("Delete passcode/RFID");
  
  //Print three entries
  if(stateMachine.GetCursorPosition()>0){
    lcd.setCursor(1,1);
    displayPasscode(stateMachine.GetCursorPosition()-1);
    lcd.setCursor(19,1);
    lcd.write(94);  //Arrow up
  }
  
  lcd.setCursor(0,2);
  lcd.write(161);  //Display the cursor
  displayPasscode(stateMachine.GetCursorPosition());
 
  if(stateMachine.GetCursorPosition()<10){
    lcd.setCursor(1,3);
    displayPasscode(stateMachine.GetCursorPosition()+1);
    lcd.setCursor(19,3);
    lcd.print('v');  //Arrow down
  }
  
}



//Display a line with a single passcode
void Display::displayPasscode(int index)
{
  byte* passcode = Settings::getStoredPasscode(index);
     
  for(int i=0; i<PASSCODELENGTH; i++){
    lcd.print(passcode[i]);
    lcd.print(' ');
  }   
}



//Display the menu to let the user change the backlight mode
void Display::displayChangeBacklightModeScreen(StateMachine stateMachine)
{
  //Display title on first line
  lcd.setCursor(0,0);
  lcd.print("___Backlight mode___");
  
  //Display the two options
  lcd.setCursor(1,1);
  lcd.print("Always on");
  lcd.setCursor(1,2);
  lcd.print("On button press");
  lcd.setCursor(1,3);
  lcd.print("Always off");
  
  //Display the cursor
  lcd.setCursor(0, stateMachine.GetCursorPosition() + 1) ;
  lcd.write(161);
}



void Display::displayFactoryResetScreen(StateMachine stateMachine)
{
  lcd.setCursor(0,0);
  lcd.print("Restore all settings");
  lcd.setCursor(0,1);
  lcd.print("to factory default?");
  lcd.setCursor(1,2);
  lcd.print("    NO     YES");
  
  if(stateMachine.GetCursorPosition()==0){
    lcd.setCursor(5,3);
    lcd.print("--");
  }
  else if(stateMachine.GetCursorPosition()==1){
    lcd.setCursor(12,3);
    lcd.print("---");
  }
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The following methods deal with the LCD backlight. They do not display anything on the LCD.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Turns on the LCD backlight whenever a key is pressed, and turns it off after a set time after 
the last key was pressed
*/
void Display::updateBacklight(AnalogKeyboard analogKeyboard)
{
  static long lastInputTime = 0;
  static bool startupLightOn = true;
  
  
  ////MODE 'ALWAYS ON'
  if (Settings::getBacklightMode() == 0)
  {
    if (!backlightOn)
    { 
      TurnBacklightOn();
    }
  }
  
  
  
  ////MODE 'ON BUTTON PRESS'
  if (Settings::getBacklightMode() == 1)  //Do the following only if backlight is not set on Always ON mode
  {  
    //Turn it on if a button was pressed (only in 'Button Press' mode)
    if (analogKeyboard.wasAnyButtonPressed())
    {  
      TurnBacklightOn();
      lastInputTime = millis();
    }
    
    //Turn it off after a few seconds with no button presses (only in 'Button Press' mode)
    if (backlightOn && (millis()-lastInputTime>BACKLIGHTTIME*1000))
    {  
      TurnBacklightOff();
    }
    
    //This part turns off the light a few seconds after powering on the Arduino (only in 'Button Press' mode)
    //(it's a bit confusing but works)
    if (startupLightOn && millis()>BACKLIGHTTIME*1000)
    {
      if(!backlightOn)digitalWrite(BACKLIGHTpin,LOW);
      startupLightOn = false;
    }    
  }
  
  
  ////MODE 'ALWAYS OFF'
  else if (Settings::getBacklightMode() == 2)
  {
    if (backlightOn)
    {
      TurnBacklightOff();
    }
  }
}



//Turns backlight on and updates the variable that tracks its state
void Display::TurnBacklightOn()
{
  backlightOn = true;
  digitalWrite(BACKLIGHTpin,HIGH);
}



//Turns backlight off and updates the variable that tracks its state
void Display::TurnBacklightOff()
{
  backlightOn = false;
  digitalWrite(BACKLIGHTpin,LOW);
}
