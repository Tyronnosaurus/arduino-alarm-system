/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This code has been developed for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.

  This is the main sketch for the Movement Detector Node. It has a PIR sensor that detects any human
  movement and sends an alert to the Central Node.
*/


///////////////////////////////////////////////////////////
////INCLUDED LIBRARIES/////////////////////////////////////
///////////////////////////////////////////////////////////
#include <SPI.h>
#include <printf.h>
#include <RF24.h>
#include <RF24Network.h>

///////////////////////////////////////////////////////////
////CONSTANTS//////////////////////////////////////////////
///////////////////////////////////////////////////////////
const int CHANNEL = 100;
const uint16_t ADDRESS = 1;  //Movement detector nodes all have address 1

const int PIR_PIN = 5;
const int LED_PIN = 3;

const int CALIBRATION_TIME = 10;  //Seconds the node will remain unactive on 
                                     //startup while it is calibrating ambient IR light

///////////////////////////////////////////////////////////
////GLOBAL VARIABLES///////////////////////////////////////
///////////////////////////////////////////////////////////
RF24 radio(9,10);
RF24Network network(radio);


///////////////////////////////////////////////////////////
///////SETUP///////////////////////////////////////////////
///////////////////////////////////////////////////////////
void setup()
{
  ////SERIAL
  Serial.begin(57600);
  printf_begin();
  
  ////Prepare I/O pins
  pinMode(PIR_PIN,INPUT);
  pinMode(LED_PIN,OUTPUT);
  
  ////Bring up the RF network
  radio.begin();
  network.begin(CHANNEL,ADDRESS);  //Also prints radio properties
  
  ////Setup PIR sensor
  CalibratePIR();

}



///////////////////////////////////////////////////////////
///////LOOP////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void loop()
{
  if(digitalRead(PIR_PIN))  //Check if the PIR has detected movement
  {   
    printf_P(PSTR("Movement detected\n"));
    digitalWrite(LED_PIN,HIGH);  //Turn ON led
    
    sendAlert();   //Send wireless message to Central Node
     
    while(digitalRead(PIR_PIN)) delay(10);  //Wait until the sensor's signal goes low
      
    digitalWrite(LED_PIN,LOW);
    
    printf_P(PSTR("Movement interrupted\n\n"));
  }
  
  delay(100);   
}



///////////////////////////////////////////////////////////
////FUNCTIONS//////////////////////////////////////////////
///////////////////////////////////////////////////////////

/*
When the sensor is powered on, it remains idle for a few seconds while the PIR sensor adapts to ambient IR light.
Try not to walk or project shadows in front of the sensor meanwhile. While this happens, the LED makes a heartbeat effect.
*/
void CalibratePIR()
{
  

  const int PERIOD = 1000;  //The LED will simulate a heartbeat with this oscillation period

  printf_P(PSTR("Calibrating..."));

  while(millis() < CALIBRATION_TIME*1000){
    analogWrite(LED_PIN, 128+127*cos(2*PI/PERIOD*millis()));  //Sets the LED's brightness following a heartbeat pattern 
  }
  
  digitalWrite(LED_PIN,LOW);
  
  printf_P(PSTR("Calibration finished\n\n"));
}



//Send an alert message to Central Node
void sendAlert()
{
  RF24NetworkHeader header(0, 'E'); //(to node, type)
  
  printf_P(PSTR("---------------------------------\n\r"));
  printf_P(PSTR("APP Sending alert to Central Node...\n\r"));
  if(network.write(header,0,0)) printf_P(PSTR("Sent ok\n"));
}
