#include <Servo.h>
#include <EnableInterrupt.h>

#define CH1_PIN 5
#define CH3_PIN 6

uint16_t pwm_value_ch1 = 0;
uint16_t pwm_value_ch3 = 0;

Servo servo_pin_8;
Servo servo_pin_9;
Servo servo_pin_10;
Servo servo_pin_11;
Servo servo_pin_12;

// see http://blog.mired.org/2015/10/a-close-look-at-pwm-input.html
// pwm value range: 1252 to 1752, middle is 1504

void change_ch1() {
  static unsigned long prev_time_ch1 = 0;
  if (digitalRead(CH1_PIN))
    prev_time_ch1 = micros();
  else
    pwm_value_ch1 = micros() - prev_time_ch1;
}

void change_ch3() {
  static unsigned long prev_time_ch3 = 0;
  if (digitalRead(CH3_PIN))
    prev_time_ch3 = micros();
  else
    pwm_value_ch3 = micros() - prev_time_ch3;
}

void setup()
{
  pinMode( 2 , OUTPUT);
  pinMode( 3 , OUTPUT);
  servo_pin_8.attach(8);
  servo_pin_9.attach(9);
  servo_pin_10.attach(10);
  servo_pin_11.attach(11);
  servo_pin_12.attach(12);

  pinMode(CH1_PIN, INPUT_PULLUP);
  enableInterrupt(CH1_PIN, &change_ch1, CHANGE);
  pinMode(CH3_PIN, INPUT_PULLUP);
  enableInterrupt(CH3_PIN, &change_ch3, CHANGE);

  Serial.begin(9600);
  while (! Serial); // Wait untilSerial is ready - Leonardo
  Serial.println("Ready");
}

void loop()
{

  uint16_t pwmin1, pwmin3;
  //noInterrupts();
  pwmin1 = pwm_value_ch1 ;
  pwmin3 = pwm_value_ch3 ;
  //interrupts(); 

  Serial.print(pwmin1);
  Serial.print("  ");
  Serial.println(pwmin3);

  if (pwmin3 > 1600) {
    digitalWrite(2 , HIGH);
    digitalWrite(3 , LOW);
    servo_pin_8.write( 0 ); // backward
    servo_pin_9.write( 0 );
  } else if (pwmin3 < 1400 && pwmin3 > 1100) {
    digitalWrite(2 , LOW);
    digitalWrite(3 , HIGH);
    servo_pin_8.write( 180 );  // forward
    servo_pin_9.write( 180 );
  } else {
    digitalWrite(2 , LOW);
    digitalWrite(3 , LOW);
    servo_pin_8.write( 95 ); // stop
    servo_pin_9.write( 95 );
  }

  if (pwmin1 > 1800) {
    servo_pin_12.write( 180 ); // left
  } else if (pwmin1 < 1200 && pwmin1 > 200) {
    servo_pin_12.write( 0 );  // right
  } else {
    servo_pin_12.write( 95 ); // stop
  }

  

  
}
