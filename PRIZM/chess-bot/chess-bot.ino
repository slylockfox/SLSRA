/*  This program will read the digital signal of the
 *  Line Finder sensor attached to digital port D3.
 *  If the sensor is facing a reflective surface and 
 *  receiving a reflected IR beam, the PRIZM red LED
 *  will switch on. If the sensor is facing a dark 
 *  surface, or too far away from a reflective surface
 *  the red LED will be off. 
 */
  
  #include <PRIZM.h>    //include the PRIZM Library
  #include <TELEOP.h>
  PRIZM prizm;          //create an object name of "prizm"
  PS4 ps4;

  int state = 0;
  
  #define SPEED_SCALE 6
  #define MAX_LIFT_POS 36000 // 38656
  #define MIN_LIFT_POS 300
  #define HANG_LIFT_POS 19000
  #define STOWED_HOOK_POS 35
  #define CENTER_HOOK_POS 115
  #define LEFT_HOOK_POS 140
  #define RIGHT_HOOK_POS 90
  #define MAX_LIFT_CURRENT_TELEOP 1600
  #define MAX_LIFT_CURRENT_INIT 1100

void setup() {          //this code runs once
  Serial.begin(9600);
  
  prizm.PrizmBegin();   //initialize PRIZM
  prizm.setMotorInvert(1, 1);
  ps4.setDeadZone (LEFT,10);     // Sets a Left Joystick Dead Zone axis range of +/- 10 about center stick
  ps4.setDeadZone(RIGHT,10);     // Sets a Right Joystick Dead Zone axis range of +/- 10 about center stick

  int battVoltage = prizm.readBatteryVoltage();
  Serial.println(battVoltage);
  if (battVoltage < 1150) {
    // battery too low to run, give flasing red
    while(true) {
      prizm.setRedLED (HIGH);
      delay(1000);
      prizm.setRedLED (LOW);
      delay(1000);
    }
  }

}

void loop() {           //this code repeats in a loop

  ps4.getPS4();

  long liftPosition = -prizm.readEncoderCount(1);
  bool buttonPressed = (prizm.readLineSensor(3) == LOW);
  prizm.setRedLED(buttonPressed);
  int liftMotorCurrent = prizm.readMotorCurrent(1);

  switch (state) { 
    
    case 0: // find starting position and reset encoders
      prizm.setServoPosition(2, STOWED_HOOK_POS);
      if(!buttonPressed && liftMotorCurrent < MAX_LIFT_CURRENT_INIT) {
        prizm.setMotorSpeed(1,-400);            // Spin DC motor 1 at a constant 200 degrees per second. The +/- sign of speed parameter determines direction
                                             // For TETRIX TorqueNADO encoders, max speed rate is approximately 600 degrees per second.
      } else  {
        prizm.setMotorSpeed(1,0); 
        prizm.resetEncoder(1);
        state = 1;
      }
      break;
      
  case 1:  // operate by remote control
      if(ps4.Connected){ 

        prizm.setGreenLED(HIGH); // indicate ready to drive

        // Serial.println(liftMotorCurrent);
        
        if (liftMotorCurrent > MAX_LIFT_CURRENT_TELEOP) {
          prizm.setMotorPower(1,125);  // stop! if button pressed or motor current too high
        } else if (ps4.Button(UP) && liftPosition < MAX_LIFT_POS) {
          prizm.setMotorSpeed(1,600);
        } else if (ps4.Button(DOWN) && liftPosition > MIN_LIFT_POS && !buttonPressed) {
          prizm.setMotorSpeed(1,-600);
        } else if (ps4.Button(TRIANGLE)) {
          prizm.setMotorTarget(1,600,MAX_LIFT_POS);
        } else if (ps4.Button(CIRCLE) && !buttonPressed) {
          prizm.setMotorTarget(1,600,MIN_LIFT_POS);
        } else if (ps4.Button(CROSS)) {
          prizm.setMotorTarget(1,600,HANG_LIFT_POS);
        } else if (!prizm.readMotorBusy(1)) {
          prizm.setMotorPower(1,125); // stop with brake
          Serial.println(liftPosition);
        }

        if (ps4.Button(LEFT)) {
          prizm.setServoPosition(2, LEFT_HOOK_POS);
        } else if (ps4.Button(RIGHT)) {
          prizm.setServoPosition(2, RIGHT_HOOK_POS);
        } 
        
      } else { // remote not connected
        prizm.setMotorPower(1,125); // stop with brake
        prizm.setServoPosition(2, CENTER_HOOK_POS);
        prizm.setGreenLED (HIGH);
        delay(1000);
        prizm.setGreenLED (LOW);
        delay(1000);

      }
      break;
  } // switch
  
  delay(50);  //slow the loop down

}
