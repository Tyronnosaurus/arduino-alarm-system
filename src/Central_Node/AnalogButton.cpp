#include "AnalogButton.h"


AnalogButton::AnalogButton(int myMin, int myMax, int myDebounceTime)
{
	min = myMin;
	max = myMax;
	debounceTime = myDebounceTime;

        auxTime = 0;
        state = 0;
}



void AnalogButton::update(int analogVal)
{
  previousState = state; 
  

  switch(state){

   //Not pressed
   case 0:
     if ((analogVal<min) || (analogVal>max))
     {

     }
     else if ((analogVal>=min) && (analogVal<=max))  //Detected possible press
     {
         state = 1;
         auxTime = millis();
     }
     break;
			
		

    //Pressed? Check if its just jitter
    case 1:
     if ((analogVal<min) || (analogVal>max))
     {
       if(millis()-auxTime<debounceTime) state = 0;
     }
     else if ((analogVal>=min) && (analogVal<=max))
     {
       if(millis()-auxTime>debounceTime) state = 2;
     }
     break;
      
      
    
    //Pressed 
    case 2:
     if ((analogVal<min) || (analogVal>max))
     {
       state = 3;
       auxTime = millis();
     }
     else if ((analogVal>=min) && (analogVal<=max))
     {
       
     }
     break;
     
     
     //Released? Check if its just jitter
     case 3:
     if ((analogVal<min) || (analogVal>max))
     {
       if(millis()-auxTime>debounceTime) state = 0;
     }
     else if ((analogVal>=min) && (analogVal<=max))
     {
       if(millis()-auxTime<debounceTime) state = 2;
     }
     break;
		

    default:
      break;
  }

}



bool AnalogButton::wasPressed()
{
  return(previousState==1 && state==2);
}



bool AnalogButton::wasReleased()
{
  return(previousState==3 && state==0);
}
