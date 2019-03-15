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

  int state = 0;

void setup() {          //this code runs once
  //Serial.begin(9600);
  
  prizm.PrizmBegin();   //initialize PRIZM
  prizm.setMotorInvert(1, 1);
  
}

void loop() {           //this code repeats in a loop

  long liftPosition;

  switch (state) {
    case 0:
  if(prizm.readLineSensor(3) == HIGH) {
    prizm.setRedLED(LOW);  // LED off
    prizm.setMotorSpeed(1,-400);            // Spin DC motor 1 at a constant 200 degrees per second. The +/- sign of speed parameter determines direction
                                         // For TETRIX TorqueNADO encoders, max speed rate is approximately 600 degrees per second.
    liftPosition = prizm.readEncoderCount(1);
    //Serial.print(liftPosition);
  } else if(prizm.readLineSensor(3) == LOW)  {
    prizm.setRedLED(HIGH); // LED on
    prizm.setMotorSpeed(1,0); 
    prizm.resetEncoder(1);
    state = 1;
  }
  break;
  case 1:
    prizm.setMotorTarget(1,600,10000);
    delay(20000);
    break;
  } // switch
  
  delay(50);  //slow the loop down

}
