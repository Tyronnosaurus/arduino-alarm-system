#include "AnalogKeyboard.h"


/*
Constructor. When an Input instance is initialized, it initializes a set of analogButton members of the class.
Afterwards, it stores which pin will be used to get the analog reading. The voltage values have been tested empirically.
The arguments are:     BTNname(min analog value, max analog value, debounceTime)
*/
AnalogKeyboard::AnalogKeyboard(int myPin) : BTNSelect(20,36,30), BTNUp(290,330,30), BTNRight(178,198,30),
                                            BTNDown(90,107,30), BTNLeft(40,61,30)
{
  pin = myPin;
}



//Must be called periodically to update the current state of the Button objects
void AnalogKeyboard::update()
{
  int val = analogRead(pin);
  
  BTNSelect.update(val);
  BTNUp.update(val);
  BTNRight.update(val);
  BTNDown.update(val);
  BTNLeft.update(val);
}



//Returns true if any of the direction keys was pressed (up, down, left or right)
bool AnalogKeyboard::wasAnyArrowPressed()
{
  return(BTNUp.wasPressed() || BTNRight.wasPressed()  ||
         BTNDown.wasPressed() || BTNLeft.wasPressed());
}



//Returns true if any key was pressed (Select, up, down, left or right)
bool AnalogKeyboard::wasAnyButtonPressed()
{
  return(BTNSelect.wasPressed() || wasAnyArrowPressed());
}	







                                   
                                
