
  #include <PRIZM.h>    //include the PRIZM Library
  #include <TELEOP.h>
  PRIZM prizm;          //create an object name of "prizm"
  PS4 ps4;
  EXPANSION exc; 

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
  #define HOOKSERVO 1
  #define LEFTSERVO 2
  #define RIGHTSERVO 3

void setup() {          //this code runs once
   Serial.begin(9600);

  pinMode(2, OUTPUT);  // LED on top of robot
  
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
      digitalWrite(2, HIGH);  // activity light
      prizm.setServoPosition(HOOKSERVO, STOWED_HOOK_POS);
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
        int receivedInput = false;
        // prizm.setGreenLED(HIGH); // indicate ready to drive

         Serial.println(liftMotorCurrent);
        
        if (liftMotorCurrent > MAX_LIFT_CURRENT_TELEOP) {
          prizm.setMotorPower(1,125);  // stop! if button pressed or motor current too high
        } else if (ps4.Button(UP) && liftPosition < MAX_LIFT_POS) {
          prizm.setMotorSpeed(1,600);
          receivedInput = true;
        } else if (ps4.Button(DOWN) && liftPosition > MIN_LIFT_POS && !buttonPressed) {
          prizm.setMotorSpeed(1,-600);
          receivedInput = true;
        } else if (ps4.Button(TRIANGLE)) {
          prizm.setMotorTarget(1,600,MAX_LIFT_POS);
          receivedInput = true;
        } else if (ps4.Button(CIRCLE) && !buttonPressed) {
          prizm.setMotorTarget(1,600,MIN_LIFT_POS);
          receivedInput = true;
        } else if (ps4.Button(CROSS)) {
          prizm.setMotorTarget(1,600,HANG_LIFT_POS);
          receivedInput = true;
        } else if (!prizm.readMotorBusy(1)) {
          prizm.setMotorPower(1,125); // stop with brake
           Serial.println(liftPosition);
        }

        if (ps4.Button(LEFT)) {
          prizm.setServoPosition(HOOKSERVO, LEFT_HOOK_POS);
          receivedInput = true;
        } else if (ps4.Button(RIGHT)) {
          prizm.setServoPosition(HOOKSERVO, RIGHT_HOOK_POS);
          receivedInput = true;
        } 

        receivedInput = receivedInput || ps4.Servo(LY) != 90 || ps4.Servo(RY) != 90;
        digitalWrite(2, receivedInput);  // activity light

        // Spark Mini motor controllers take servo angles
        prizm.setServoPosition(LEFTSERVO, 180 - ps4.Servo(LY));
        prizm.setServoPosition(RIGHTSERVO, ps4.Servo(RY));
        
      } else { // remote not connected
        prizm.setMotorPower(1,125); // stop with brake
        prizm.setServoPosition(HOOKSERVO, CENTER_HOOK_POS);
        digitalWrite(2, LOW);  // activity light
        prizm.setGreenLED (HIGH);
        delay(1000);
        prizm.setGreenLED (LOW);
        delay(1000);

      }
      break;
  } // switch
  
  delay(50);  //slow the loop down

}
