
  #include <PRIZM.h>    //include the PRIZM Library
  #include <TELEOP.h>

  #include <Wire.h>     // I2C comm
  
  #include "rgb_lcd.h"  // 16x2 display
  const int colorR = 230; const int colorG = 100; const int colorB = 0;   // beginning backlight color

  PRIZM prizm;          //create an object name of "prizm"
  PS4 ps4;
  EXPANSION exc; 

  rgb_lcd lcd;  // 16x2 display

  int state = 0;
  int battVoltage = 0;
  
  #define SPEED_SCALE 6
  #define MAX_LIFT_POS -31200 // 38656, 36000
  #define MIN_LIFT_POS -300
  #define HANG_LIFT_POS -16000
  #define STOWED_HOOK_POS 35
  #define CENTER_HOOK_POS 115
  #define LEFT_HOOK_POS 140
  #define RIGHT_HOOK_POS 90
  #define MAX_LIFT_CURRENT_TELEOP 1600
  #define MAX_LIFT_CURRENT_INIT 1500
  #define HOOKSERVO 1
  #define LEFTSERVO 2
  #define RIGHTSERVO 3
  #define LIFT_UP_SPEED -600
  #define LIFT_DOWN_SPEED 600
  #define LIFT_STOW_SPEED 400
  #define GAMEPAD_DEAD_ZONE 30

void setup() {          //this code runs once
  
  // Serial.begin(9600);

  pinMode(2, OUTPUT);  // LED on top of robot

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("Hi, I'm ChessBot!");
  lcd.setCursor(0, 1);
  lcd.print("Press green btn");

  prizm.PrizmBegin();   //initialize PRIZM
  prizm.setMotorInvert(1, 1);
  ps4.setDeadZone (LEFT,GAMEPAD_DEAD_ZONE);     // Sets a Left Joystick Dead Zone axis range of +/- 10 about center stick
  ps4.setDeadZone(RIGHT,GAMEPAD_DEAD_ZONE);     // Sets a Right Joystick Dead Zone axis range of +/- 10 about center stick

  battVoltage = prizm.readBatteryVoltage();
  // Serial.println(battVoltage);
  if (battVoltage < 1150) {
    // battery too low to run, give flasing red
    lcd.home(); lcd.clear();
    lcd.setRGB(255, 0, 0);  // red backlight
    lcd.print("Battery LOW");
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
  battVoltage = prizm.readBatteryVoltage();

  char msg[20];

  switch (state) { 

    case 0:
      lcd.home(); lcd.clear();
      lcd.setRGB(0, 0, 255);  // blue backlight
      lcd.print("Chessbot Initing");
      state = 1;
      break;

    case 1: // find starting position and reset encoders
      lcd.setCursor(0, 1); // line 2 for current
      sprintf(msg, "%04d0mV %04dmA", battVoltage, liftMotorCurrent);
      lcd.print(msg);
      prizm.setServoPosition(HOOKSERVO, STOWED_HOOK_POS);
      if(!buttonPressed && liftMotorCurrent < MAX_LIFT_CURRENT_INIT) {
        prizm.setMotorSpeed(1,LIFT_STOW_SPEED);            // Spin DC motor 1 at a constant 200 degrees per second. The +/- sign of speed parameter determines direction
                                             // For TETRIX TorqueNADO encoders, max speed rate is approximately 600 degrees per second.
      } else  {
        prizm.setMotorSpeed(1,0); 
        prizm.resetEncoder(1);
        state = 2;  // done initializing, start running
      }
      break;
      
    case 2:
      lcd.home(); lcd.clear();
      lcd.setRGB(0, 255, 0);  // green backlight
      lcd.print("Chessbot Running");
      state = 3;
      break;

  case 3:  // operate by remote control
      if(ps4.Connected){ 
        int receivedInput = false;

        // Serial.println(liftMotorCurrent);
        
        if (liftMotorCurrent > MAX_LIFT_CURRENT_TELEOP) {
          prizm.setMotorPower(1,125);  // stop! if button pressed or motor current too high
        } else if (ps4.Button(UP) && liftPosition > MAX_LIFT_POS) {
          prizm.setMotorSpeed(1,LIFT_UP_SPEED);
          receivedInput = true;
        } else if (ps4.Button(DOWN) && liftPosition < MIN_LIFT_POS && !buttonPressed) {
          prizm.setMotorSpeed(1,LIFT_DOWN_SPEED);
          receivedInput = true;
        } else if (ps4.Button(TRIANGLE)) {
          prizm.setMotorTarget(1,LIFT_DOWN_SPEED,MAX_LIFT_POS);
          receivedInput = true;
        } else if (ps4.Button(CIRCLE) && !buttonPressed) {
          prizm.setMotorTarget(1,LIFT_DOWN_SPEED,MIN_LIFT_POS);
          receivedInput = true;
        } else if (ps4.Button(CROSS)) {
          prizm.setMotorTarget(1,LIFT_DOWN_SPEED,HANG_LIFT_POS);
          receivedInput = true;
        } else if (!prizm.readMotorBusy(1)) {
          prizm.setMotorPower(1,125); // stop with brake
          // Serial.println(liftPosition);
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

        lcd.setCursor(0, 1); // line 2 for position
        sprintf(msg, "%04d0mV %6ld", battVoltage, liftPosition);
        lcd.print(msg);

        // Spark Mini motor controllers take servo angles
        prizm.setServoPosition(LEFTSERVO, 180 - ps4.Servo(LY));
        prizm.setServoPosition(RIGHTSERVO, ps4.Servo(RY));
        
      } else { // remote not connected
        prizm.setMotorPower(1,125); // stop with brake
        prizm.setServoPosition(HOOKSERVO, CENTER_HOOK_POS);
        lcd.setCursor(0, 1);
        lcd.print("PS4 not connectd");
        prizm.setGreenLED (HIGH);
        delay(1000);
        prizm.setGreenLED (LOW);
        delay(1000);
        lcd.home();  lcd.clear();
        lcd.print("Chessbot Running");
      }
      break;
  } // switch
  
  delay(50);  //slow the loop down

}
