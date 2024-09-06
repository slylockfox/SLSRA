#include <Servo.h>
#include <EnableInterrupt.h>

#define CH6_PIN 5 // knob 4
#define CH1_PIN 6 // strafe 5
#define CH2_PIN 4 // fwd/rev 6

enum Motion {
  forward,
  backward,
  left,
  right,
  forleft,
  forright,
  backleft,
  backright
};

uint16_t pwm_value_ch1 = 0;
uint16_t pwm_value_ch2 = 0;
uint16_t pwm_value_ch6 = 0;

Servo servo_pin_8_FL;
Servo servo_pin_9_FR;
Servo servo_pin_10_BR;
Servo servo_pin_11_BL;

// see http://blog.mired.org/2015/10/a-close-look-at-pwm-input.html
// pwm value range: 1252 to 1752, middle is 1504

void change_ch1() {
  static unsigned long prev_time_ch1 = 0;
  if (digitalRead(CH1_PIN))
    prev_time_ch1 = micros();
  else
    pwm_value_ch1 = micros() - prev_time_ch1;
}

void change_ch2() {
  static unsigned long prev_time_ch2 = 0;
  if (digitalRead(CH2_PIN))
    prev_time_ch2 = micros();
  else
    pwm_value_ch2 = micros() - prev_time_ch2;
}

void change_ch6() {
  static unsigned long prev_time_ch6 = 0;
  if (digitalRead(CH6_PIN))
    prev_time_ch6 = micros();
  else
    pwm_value_ch6 = micros() - prev_time_ch6;
}

void setup()
{
  pinMode( 2 , OUTPUT);
  pinMode( 3 , OUTPUT);
  servo_pin_8_FL.attach(8);
  servo_pin_9_FR.attach(9);
  servo_pin_10_BR.attach(10);
  servo_pin_11_BL.attach(11);

  pinMode(CH1_PIN, INPUT_PULLUP);
  enableInterrupt(CH1_PIN, &change_ch1, CHANGE);
  pinMode(CH2_PIN, INPUT_PULLUP);
  enableInterrupt(CH2_PIN, &change_ch2, CHANGE);
  pinMode(CH6_PIN, INPUT_PULLUP);
  enableInterrupt(CH6_PIN, &change_ch6, CHANGE);

  Serial.begin(9600);
  while (! Serial); // Wait untilSerial is ready - Leonardo
  Serial.println("Ready");
}

int scale_pwm_to_servo (int pwm) {
  float result = (1504.0 - pwm) * 30.0 / 250.0; // range of -90 deg to +90 deg
  return (int)result;
}

int dead_zone (int servo_setting) {
  return (abs(servo_setting) > 10) ? servo_setting : 0;
}

int clip_to_positive (int servo_setting) {
  return (servo_setting > 0) ? servo_setting : 0;
}

int clip_to_negative (int servo_setting) {
  return (servo_setting < 0) ? servo_setting : 0;
}

void loop()
{

  uint16_t pwmin1, pwmin2, pwmin6;
  //noInterrupts();
  pwmin1 = pwm_value_ch1 ; // strafe
  pwmin2 = pwm_value_ch2 ; // fwd/rev
  pwmin6 = pwm_value_ch6 ; // squat
  //interrupts(); 

  Serial.print(pwmin1);
  Serial.print("  ");
  Serial.print(pwmin2);
  Serial.print("  ");
  Serial.println(pwmin6);

  // assume only one axis of motion
  int forward_servo = dead_zone(scale_pwm_to_servo(pwmin2));
  int sideways_servo = dead_zone(scale_pwm_to_servo(pwmin1));
  int squat_servo = dead_zone(scale_pwm_to_servo(pwmin6));

  if (forward_servo > 0) {
     digitalWrite(2 , HIGH);
   digitalWrite(3 , LOW);
    servo_pin_8_FL.write( 90 + forward_servo);
    servo_pin_9_FR.write( 90 - forward_servo);
    servo_pin_10_BR.write( 90 );
    servo_pin_11_BL.write( 90 );
  } else if (forward_servo < 0) {
     digitalWrite(2 , HIGH);
    digitalWrite(3 , LOW);
    servo_pin_8_FL.write( 90 );
    servo_pin_9_FR.write( 90 );
    servo_pin_10_BR.write( 90 - forward_servo);
    servo_pin_11_BL.write( 90 + forward_servo);
  } else if (sideways_servo > 0) {
     digitalWrite(3 , HIGH);
    digitalWrite(2 , LOW);
    servo_pin_8_FL.write( 90 );
    servo_pin_9_FR.write( 90 - sideways_servo);
    servo_pin_10_BR.write( 90 + sideways_servo );
    servo_pin_11_BL.write( 90 );
  } else if (sideways_servo < 0) {
         digitalWrite(3 , HIGH);
    digitalWrite(2 , LOW);
    servo_pin_8_FL.write( 90 - sideways_servo);
    servo_pin_9_FR.write( 90 );
    servo_pin_10_BR.write( 90 );
    servo_pin_11_BL.write( 90 + sideways_servo );
  } else { // no robot motion: obey squat setting
    digitalWrite(2 , LOW);
   digitalWrite(3 , LOW);
    servo_pin_8_FL.write( 90 + clip_to_positive(squat_servo));
    servo_pin_9_FR.write( 90 - clip_to_positive(squat_servo));
    servo_pin_10_BR.write( 90 + clip_to_positive(squat_servo));
    servo_pin_11_BL.write( 90 - clip_to_positive(squat_servo));
  }

  if (squat_servo > 0) {
    digitalWrite(2 , HIGH); // yellow
    digitalWrite(3 , LOW); // green
  } else if (squat_servo < 0) {
    digitalWrite(2 , LOW);
    digitalWrite(3 , HIGH);
  } else {
    digitalWrite(2 , LOW);
    digitalWrite(3 , LOW);
  }

  
  
}
