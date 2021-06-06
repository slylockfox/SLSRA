#include <Servo.h>

#include <Ultrasonic.h>

Ultrasonic ultrasonic(4);

int _ABVAR_1_distance = 0 ;
Servo servo_pin_10;
Servo servo_pin_11;

void setup()
{
  pinMode( 2 , OUTPUT);
  pinMode( 3 , OUTPUT);
  servo_pin_10.attach(10);
  servo_pin_11.attach(11);
  digitalWrite(2 , HIGH);



  _ABVAR_1_distance = 0 ;

}

void loop()
{
  servo_pin_10.write( 50 );
  servo_pin_11.write( 100 );
  delay( 700 );
  _ABVAR_1_distance = ultrasonic.MeasureInCentimeters();
  if (( ( _ABVAR_1_distance ) < ( 7 ) ))
  {
    digitalWrite(3 , HIGH);
    servo_pin_10.write( 70 );
    delay( 700 );
    servo_pin_11.write( 52 );
    delay( 800 );
    servo_pin_10.write( 50 );
    delay( 700 );
  }
  else
  {
    digitalWrite(3 , LOW);
  }
}
