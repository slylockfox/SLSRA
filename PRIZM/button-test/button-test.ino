/*  This program will read the digital signal of the
 *  Line Finder sensor attached to digital port D3.
 *  If the sensor is facing a reflective surface and 
 *  receiving a reflected IR beam, the PRIZM red LED
 *  will switch on. If the sensor is facing a dark 
 *  surface, or too far away from a reflective surface
 *  the red LED will be off. 
 */
  
#include <PRIZM.h>    //include the PRIZM Library
PRIZM prizm;          //create an object name of "prizm"

// #define buttonPin 3    

void setup() {          //this code runs once
  // pinMode(buttonPin, INPUT);
  prizm.PrizmBegin();   //initialize PRIZM
  
}

void loop() {           //this code repeats in a loop

  // int buttonState = digitalRead(buttonPin);
  
  if(prizm.readLineSensor(5) == HIGH) {prizm.setRedLED(LOW);}  // LED off

  if(prizm.readLineSensor(5) == LOW)  {prizm.setRedLED(HIGH);} // LED on
  
  delay(50);  //slow the loop down

}
