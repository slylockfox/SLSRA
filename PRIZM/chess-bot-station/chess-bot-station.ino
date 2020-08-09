
  #include <PRIZM.h>    //include the PRIZM Library
  #include <TELEOP.h>
  
  #undef GREEN // these colors are also defined in TELEOP, probably for LEDs in that module
  #undef BLUE
  #include "rgb_lcd.h"  // Grove 16x2 display
  #include "TM1637.h" // Grove 4-digit display

  #define GAMEPAD_DEAD_ZONE 38
  #define LCD_DELAY 150
  #define LEFT_MOTOR 1
  #define RIGHT_MOTOR 2
  #define DISPLAY_CLK 2
  #define DISPLAY_DIO 3
  #define ROTARY_ANGLE_SENSOR A1
  
  const int colorR = 230; const int colorG = 100; const int colorB = 0;   // beginning backlight color
  char OPEN_STR[] = "OPEN";

  PRIZM prizm;          //create an object name of "prizm"
  PS4 ps4;
  // EXPANSION exc; 

  rgb_lcd lcd;  // Grove 16x2 display

  TM1637 tm1637(DISPLAY_CLK, DISPLAY_DIO); // Grove 4-digit display
 
  int state = 0;
  int battVoltage = 0;
  unsigned long timer = 0;
  int speedMultiplier = 18; // 180 degrees per sec, or quarter of max
  bool closedLoop = true; // use speed commands with encoders (closed loop) or straight power commands

String BatteryMsg (int v) {
  String msg = "Batt V: "; 
  int vMantissa = v / 100;
  int vDecimal = v - vMantissa * 100;
  return msg + vMantissa + "." + vDecimal + "    ";
}

void setup() {          //this code runs once
  
  // Serial.begin(9600);

  timer = millis();
  battVoltage = prizm.readBatteryVoltage();

  // Grove 4-digit display
  tm1637.init();
  tm1637.set(BRIGHTEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

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
    lcd.home();
    lcd.setRGB(255, 0, 0);  // red backlight
    lcd.print(BatteryMsg(battVoltage)); delay (LCD_DELAY);
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
        lcd.home();
        lcd.setRGB(0, 255, 0);  // green backlight
        lcd.print("Station Running."); delay (LCD_DELAY);
        state = 1;
        
      } else { // remote not connected
        prizm.setMotorPower(LEFT_MOTOR,125); // stop with brake
        prizm.setMotorPower(RIGHT_MOTOR,125); // stop with brake
        lcd.setRGB(0, 0, 255);  // blue backlight
        lcd.setCursor(0, 1);
        lcd.print("PS4 not connectd"); delay (LCD_DELAY);
        prizm.setGreenLED (HIGH);
        delay(1000);
        prizm.setGreenLED (LOW);
        delay(1000);
        
      }
      
      break;

    case 1:

      if(ps4.Connected){ 
        // read joysticks
        int y = ps4.Motor(RY);
        int x = ps4.Motor(RX);

        // right joystick only, combine X and Y
        int leftSpeed = y + x;
        int rightSpeed = y - x;

        // display battery voltage if idle
        if (abs(leftSpeed) > 0 || abs(rightSpeed) > 0) { timer = millis(); }  // time 10 seconds of idle time
        else if (millis() - timer > 10000) { // update every 10 seconds
          timer = millis();
          battVoltage = prizm.readBatteryVoltage();
          lcd.setCursor(0, 1);
          lcd.print(BatteryMsg(battVoltage)); delay (LCD_DELAY);
        }
        
        if (leftSpeed == 0 && rightSpeed == 0) {
          // break stop
          prizm.setMotorPowers(125, 125);
          prizm.setRedLED (HIGH); prizm.setGreenLED (LOW);
          // tm1637.displayStr("STOP");
          int rotarySensor = analogRead(ROTARY_ANGLE_SENSOR);
          speedMultiplier = (float)rotarySensor * .0704; // * 72 / 1023
          if (speedMultiplier < 5) {closedLoop = false; tm1637.displayStr(OPEN_STR);}
          else {closedLoop = true; tm1637.displayNum(speedMultiplier * 10);}
        } else {
          prizm.setRedLED (LOW); prizm.setGreenLED (HIGH);
          if (closedLoop) {
            leftSpeed = speedMultiplier * leftSpeed / 10; 
            rightSpeed = speedMultiplier * rightSpeed / 10; 
          }
          // slow or turbo
          if (ps4.Button(L1) || ps4.Button(L2) || ps4.Button(R1) || ps4.Button(R2)) {
            leftSpeed *= 3; rightSpeed *= 3; // turbo
          } else if (ps4.Button(UP) || ps4.Button(DOWN) || ps4.Button(LEFT) || ps4.Button(RIGHT)) {
            leftSpeed *= 2; rightSpeed *= 2; // light turbo
          }
          // tm1637.displayNum(leftSpeed); // display forward velocity of one motor
          if (closedLoop) {
            prizm.setMotorSpeeds(leftSpeed, rightSpeed);
          } else { // open loop, no encoders
            prizm.setMotorPowers(leftSpeed, rightSpeed);
          }
        }
        
      } else { // ps4 not connected
        state = 0;
      }
      break;

    } // switch on state

  delay(10);  //slow the loop down

}
