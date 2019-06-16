
  #include <PRIZM.h>    //include the PRIZM Library
  #include <TELEOP.h>
  
  PRIZM prizm;          //create an object name of "prizm"
  PS4 ps4;

  int state = 0;
  int battVoltage = 0;
  
  #define SPEED_SCALE 7.2

void setup() {          //this code runs once
  
  // Serial.begin(9600);

  prizm.PrizmBegin();   //initialize PRIZM
  prizm.setMotorInvert(2, 1);
  ps4.setDeadZone (LEFT,50);     // Sets a Left Joystick Dead Zone axis range of +/- 10 about center stick
  ps4.setDeadZone(RIGHT,20);     // Sets a Right Joystick Dead Zone axis range of +/- 10 about center stick

  battVoltage = prizm.readBatteryVoltage();
  // Serial.println(battVoltage);
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

      if(ps4.Connected){ 
        prizm.setGreenLED (HIGH);
        
        // motor 1 is right mmotor
        //int leftSpeed = max(-100, min(100, ps4.Motor(LY) + 2 * ps4.Motor(RX))) * SPEED_SCALE;
        //int rightSpeed = max(-100, min(100, ps4.Motor(LY) - 2 * ps4.Motor(RX))) * SPEED_SCALE;
        int leftSpeed = min(100, ps4.Motor(RY) + ps4.Motor(RX)) * SPEED_SCALE;
        int rightSpeed = min(100, ps4.Motor(RY) - ps4.Motor(RX)) * SPEED_SCALE;
        // char msg[100];  sprintf(msg, "%d %d", leftSpeed, rightSpeed);  Serial.println(msg);
        prizm.setMotorSpeeds(rightSpeed, leftSpeed);

      } else { // remote not connected
        prizm.setMotorPowers(125,125); // stop with brake
        // blink green light on/off
        prizm.setGreenLED (HIGH);
        delay(1000);
        prizm.setGreenLED (LOW);
        delay(1000);
      }
 
  
  delay(50);  //slow the loop down

}
