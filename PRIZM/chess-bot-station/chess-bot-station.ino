
  #include <PRIZM.h>    //include the PRIZM Library
  #include <TELEOP.h>
  
  #undef GREEN // these colors are also defined in TELEOP, probably for LEDs in that module
  #undef BLUE
  #include "rgb_lcd.h"  // Grove 16x2 display
  //#include "TM1637.h" // Grove 4-digit display

  #define GAMEPAD_DEAD_ZONE 35
  #define LCD_DELAY 50
  #define LEFT_MOTOR 1
  #define RIGHT_MOTOR 2
  #define DISPLAY_CLK 4
  #define DISPLAY_DIO 5
  
  const int colorR = 230; const int colorG = 100; const int colorB = 0;   // beginning backlight color

  PRIZM prizm;          //create an object name of "prizm"
  PS4 ps4;
  // EXPANSION exc; 

  rgb_lcd lcd;  // Grove 16x2 display

  //TM1637 tm1637(DISPLAY_CLK, DISPLAY_DIO); // Grove 4-digit display
 
  int state = 0;
  int battVoltage = 0;
  unsigned long timer = 0;

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

  // Grove 4-digit display will show forward velocity in degrees/sec
  //tm1637.init();
  //tm1637.set(BRIGHTEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);  // yellow light
  lcd.print(BatteryMsg(battVoltage)); delay (LCD_DELAY);
  lcd.setCursor(0, 1);
  lcd.print("Press green btn."); delay (LCD_DELAY);

  prizm.PrizmBegin();   //wait for green button press
  
  prizm.setMotorInvert(2, 1);  // invert right motor
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
        lcd.print("Station Running"); delay (LCD_DELAY);
        
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

        if (leftSpeed == 0) { // if forward speed is 0, steering is rotating
          leftSpeed = steer;
          rightSpeed = -steer;
        } else { // reduce one side by percentage of steer
          if (steer < 0) {
            leftSpeed += steer;
          } else if (steer > 0) {
            rightSpeed -= steer;
          }
        }

        // display battery voltage if idle
        if (abs(leftSpeed) > 0 || abs(steer) > 0) { timer = millis(); }  // time 10 seconds of idle time
        else if (millis() - timer > 10000) { // update every 10 seconds
          timer = millis();
          battVoltage = prizm.readBatteryVoltage();
          lcd.home(); lcd.clear();
          lcd.print("Station Running"); delay (LCD_DELAY);
          lcd.setCursor(0, 1);
          lcd.print(BatteryMsg(battVoltage)); delay (LCD_DELAY);
        }
        
        // specify motor speed in degrees/sec
        leftSpeed = 7.2 * leftSpeed;  // max speed = 720 DPS
        rightSpeed = 7.2 * rightSpeed; 
        // tm1637.displayNum(leftSpeed); // display forward velocity of one motor
        prizm.setMotorSpeed(LEFT_MOTOR, leftSpeed);
        prizm.setMotorSpeed(RIGHT_MOTOR, rightSpeed);
        
      } else { // ps4 not connected
        state = 0;
      }
      break;

    } // switch on state

  delay(10);  //slow the loop down

}
