
  #include <PRIZM.h>    //include the PRIZM Library
  #include <TELEOP.h>
  
  #undef GREEN // these colors are also defined in TELEOP, probably for LEDs in that module
  #undef BLUE
  #include "rgb_lcd.h"  // 16x2 display

  const int colorR = 230; const int colorG = 100; const int colorB = 0;   // beginning backlight color

  PRIZM prizm;          //create an object name of "prizm"
  PS4 ps4;
  // EXPANSION exc; 

  rgb_lcd lcd;  // 16x2 display
 
  int state = 0;
  int battVoltage = 0;
  unsigned long timer = 0;
  
  #define GAMEPAD_DEAD_ZONE 30
  #define LEFT_MOTOR 1
  #define RIGHT_MOTOR 2

String BatteryMsg (int v) {
  String msg = "Batt V: "; 
  int vMantissa = v / 100;
  int vDecimal = v - vMantissa * 100;
  return msg + vMantissa + "." + vDecimal;
}

void setup() {          //this code runs once
  
  // Serial.begin(9600);

  timer = millis();
  battVoltage = prizm.readBatteryVoltage();
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);  // yellow light
  lcd.print(BatteryMsg(battVoltage));
  lcd.setCursor(0, 1);
  lcd.print("Press green btn.");

  prizm.PrizmBegin();   //wait for green button press
  
  prizm.setMotorInvert(1, 1);
  ps4.setDeadZone (LEFT,GAMEPAD_DEAD_ZONE);     // Sets a Left Joystick Dead Zone axis range of +/- 10 about center stick
  ps4.setDeadZone(RIGHT,GAMEPAD_DEAD_ZONE);     // Sets a Right Joystick Dead Zone axis range of +/- 10 about center stick

  // Serial.println(battVoltage);
  if (battVoltage < 1150) {
    // battery too low to run, give flasing red
    lcd.home(); lcd.clear();
    lcd.setRGB(255, 0, 0);  // red backlight
    lcd.print(BatteryMsg(battVoltage));
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

    switch (state) { 

    case 0:

      if(ps4.Connected){ 
        
        battVoltage = prizm.readBatteryVoltage();

      lcd.home(); lcd.clear();
      lcd.setRGB(0, 255, 0);  // green backlight
      lcd.print("Station Running");

      } else { // remote not connected
        prizm.setMotorPower(LEFT_MOTOR,125); // stop with brake
        prizm.setMotorPower(RIGHT_MOTOR,125); // stop with brake
        lcd.setRGB(0, 0, 255);  // blue backlight
        lcd.setCursor(0, 1);
        lcd.print("PS4 not connectd");
        prizm.setGreenLED (HIGH);
        delay(1000);
        prizm.setGreenLED (LOW);
        delay(1000);
        lcd.home();  lcd.clear();
        lcd.print("Station Running");
      }

      
      state = 1;
      break;

   

    case 1:

      if(ps4.Connected){ 
        // read joysticks
        int leftSpeed = ps4.Motor(LY);
        int rightSpeed = leftSpeed;
        int steer = ps4.Motor(RX);

        // reduce one side by percentage of steer
        if (steer > 0) {
          leftSpeed -= steer;
        } else if (steer < 0) {
          rightSpeed += steer;
        }

        // display battery voltage if idle
        if (leftSpeed == 0 && steer == 0) {
          if (millis() - timer > 10000) { // update every 10 seconds
              battVoltage = prizm.readBatteryVoltage();
                  lcd.home(); lcd.clear();
      lcd.print("Station Running");
        lcd.setCursor(0, 1);
  lcd.print(BatteryMsg(battVoltage));
  timer = millis();
          }
        }

        
        // specify motor speed in degrees/sec
        leftSpeed = 7.2 * leftSpeed;  // max speed = 720 DPS
        rightSpeed = 7.2 * rightSpeed; 
        prizm.setMotorSpeed(LEFT_MOTOR, leftSpeed);
        prizm.setMotorSpeed(RIGHT_MOTOR, rightSpeed);
        
      } else { // ps4 not connected
        state = 0;
      }
      break;

    } // switch on state

  
  delay(50);  //slow the loop down

}