/*
  Copyright (C) 2015 E. Tiron <Eduard.Tiron@gmail.com>
  This code has been developed for the final project of my career, an Arduino Smart Alarm. You may reuse all or part 
  of the included code freely for non-commercial purposes.

  This library is used to generate sounds and short melodies for the piezo in the Key Tray Node. They are used to
  give the user feedback whenever he intracts with the device.
*/

#include "Sounds.h"

Sounds::Sounds()
{
  
}


void Sounds::KeyPressed()
{ 
  tone(PIEZOpin, NOTE_B3, 50);
  delay(50);
  noTone(PIEZOpin);
}


void Sounds::RFIDdetected()
{
  KeyPressed(); //No need for a special sound
}


void Sounds::AccessGranted()
{
  delay(300);  //Give time to Sound_KeyPressed() or Sound_RFIDdetected to finish
  noTone(PIEZOpin);
  int melody[] = {NOTE_G3, NOTE_B3, NOTE_C4};
  int noteDurations[] = {6,6,4};
  int length = 3;
  playMelody(melody,noteDurations,length);
}


void Sounds::AccessDenied()
{
  delay(300);  //Give time to Sound_KeyPressed() or Sound_RFIDdetected to finish
  noTone(PIEZOpin);
  int melody[] = {NOTE_G3, NOTE_C4, NOTE_B3};
  int noteDurations[] = {6,6,4};
  int length = 3;
  playMelody(melody,noteDurations,length); 
}


void Sounds::AlarmActivated()
{
  delay(300);  //Give time to Sound_KeyPressed()
  int melody[] = {NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6};
  int noteDurations[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};
  int length = 15;
  playMelody(melody,noteDurations,length); 
}


void Sounds::AddingNewPasscode()
{
  int melody[] = {NOTE_G3, NOTE_G3, NOTE_C5, NOTE_D5};
  int noteDurations[] = {8,8,4,4};
  int length = 4;
  playMelody(melody,noteDurations,length);
}


void Sounds::SentNewPasscode()
{
  delay(300);  //Give time to Sound_KeyPressed()
  int melody[] = {NOTE_G3, NOTE_G3, NOTE_C5, NOTE_G3, NOTE_D5};
  int noteDurations[] = {8,8,4,6,2};
  int length = 5;
  playMelody(melody,noteDurations,length);
}


void Sounds::Transmittion()
{
   tone(PIEZOpin, NOTE_B2, 15); 
}


void Sounds::CentralNodeUnresponsive()
{
  noTone(PIEZOpin);
  int melody[] = {NOTE_G6, NOTE_A5, NOTE_G6, NOTE_A5};
  int noteDurations[] = {3,3,3,3};
  int length = 4;
  playMelody(melody,noteDurations,length);
 
} 


/*
Plays a melody with the data provided.
*/
void Sounds::playMelody(int melody[], int noteDurations[], int length)
{
  //Iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    //To calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(PIEZOpin, melody[thisNote],noteDuration);

    //To distinguish the notes, set a minimum time between them.
    //The note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    //Stop the tone playing:
    noTone(PIEZOpin);
  }
}
