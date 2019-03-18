// Arduino I2C Master over HiTechnic Motor Controller as Slave
// by Francesca Bragg
// Created 2 July 2014
//
// Turns the Arduino to a I2C master device using the Wire library. 
// Send read and write commands to the I2C Arduino slave to set and control motors.
//
// Components:
// Arduino Uno (I2C master)
// HiTechnic Motor Controller (I2C slave) and power source
// *optional: other devices/sensors that can be replaced/added as slaves, we attempted to connect to the ultrasonic sensor and were successful in communicating with it, however couldn't get it to function
// Breadboard Adapter for LEGO MINDSTORMS 
// Breadboard/wires
// 9v Battery
//
// Physical Setup:
// Arduino Uno is connected to breadboard adapter using SDA, SLC, power, and ground pins
// Breadboad adapter is connected to the motor controller using a standard NXT modular sensor cable
// HiTechnic motor conrollers are connected to motor 1 and 2 using twisted pair wire
//
// Connections:
// A0-A3 - Not in use
// A4 - IN USE as SDA
// A5 - IN USE as SCL
// GND - IN USE to connect ground (GND to GND)
// 5v - IN USE to connect power (5v to 5v)
//
// Motor Configuration:
// Motor 1 is the left motor and Motor 2 is the right motor
// Note that motor 2 is motor 1 reversed so forward commands are - and reverse + which is why we make the percent value negative
//
// TWI Changes:
// We had to slow the communication rate of the I2C bus from 100 khz to about 9.6 khz it would "look" like the NXT controller to slave devices
// This was done by modifying the TWI_FREQ constant within our source code
// Within the Twi.h file, this conditional compilation that controlls the compilation speed, allowed us to do that:
//     #ifndef TWI_FREQ
//     #define TWI_FREQ 100000L
//     #endif
// It allows us to specify a new speed, we used 37390, or use the existing constant (100000) if one isn't specified.
// Additionally, the prescalar value had to be changed to 4 within the source code by changing Cbi to Sbi or clear bit value (0) to set bit value (1) 
// because the way the TWI library previously setup the prescalar, you could only set the baud rate so low.
// By changing our prescalar value to four, we allowed for a greater scope of accepted values.
// Here is the modified section of code:
//    // initialize twi prescalar and bit rate
//    #if PRESCALAR == 4
//    sbi(TWSR, TWPS0);	//replaced cbi with sbi
//    #else
//    cbi(TWSR, TWPS0); 
//    #endif
//    cbi(TWSR, TWPS1);
//    TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
// This changes the speed to become 9615.38 baud because:
//  using the equation in the twi library to find the TWBR we get:((16,000,000/37390)-16)/2* = 206
//  then we plug this into the SCL frequency equation which is: 
//  CPU Clock frequency/16 + 2(TWBR)*4^TWPS = 16,000,000/16 + 2(206) *4^1 = 16,000,000/ 16+ 412 * 4 = 16,000,000/ 16+ 1648= 16,000,000/1664 = 9615.38
// which allows everything to work because it slows everything down close to the normal operating speed 9600 baud (what the NXT operates at)
//
// Addresses:
// The device address is not byte shifted, that is happening for us inside the wire library somewhere.
// Device uses address 0x05 not 0x02 and 0x03 for write and read
// 
// Memory Map for the HiTechnic Controller is as follows: 
// 0x00 yields the version number (example: "V2.0")
// 0x08 yields the manufacturer (example: "Hitechnc") 
// 0x10 yields the sensor type (example: "MotorCon")
// All of the locations above return 8 bytes defined as NUM_BYTES
// Motor 1 power is at address 0x45 and Motor 2 power at 0x46

#define TWI_FREQ 37390L  // reduce communication rate of I2C bus to 38.4 khz 
#include <Wire.h>

#define DEVICE_ADDRESS 0x05
#define REG_MANUFACTURER 0x08
#define REG_VERSION 0x00
#define REG_SENSOR_TYPE 0x10
#define NUM_BYTES 8
#define M1_POWER 0x45
#define M2_POWER 0x46

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
int getManufacturer(char *s) // return length of s
{
 int i = 0; // declare variable i which represents the index into the array; arrays begin at 0 which is why we set i=0
 // Read from device
 Wire.beginTransmission(DEVICE_ADDRESS); //begin transmission at DEVICE_ADDRESS (connect to device 0x05
 Wire.write(REG_MANUFACTURER); // write to the internal register you want to read; the manufacturer
 Wire.endTransmission(1); // send stop sequence (true(1)); master will release I2C bus

 Wire.requestFrom(DEVICE_ADDRESS, NUM_BYTES); // request 8 bytes from DEVICE_ADDRESS
 while (Wire.available()) // while available,
 {
   s[i++] = Wire.read(); // i++ increments i so we can store the new data in a sequential space on the array
 }
 s[i] = '\0'; // all strings in C are terminated with a zero (null character); need zero because zeros terminate strings and tells the print ln when to stop.
 return i; // length of the string contained in variable i 
}

int getSensorType(char *s)// return length of s
{
 int i; // declare variable i which represents the index into the array; arrays begin at 0 which is why we set i=0
 // Read from device
 Wire.beginTransmission(DEVICE_ADDRESS); //begin transmission at DEVICE_ADDRESS (connect to device 0x05
 Wire.write(REG_SENSOR_TYPE); // write to the internal register you want to read; the sensor type
 Wire.endTransmission(1); // send stop sequence (true(1)); master will release I2C bus

 Wire.requestFrom(DEVICE_ADDRESS, NUM_BYTES); // request 8 bytes from DEVICE_ADDRESS
 while (Wire.available()) // while available,
 {
   s[i++] = Wire.read(); // i++ increments i so we can store the new data in a sequential space on the array
 }
 s[i] = '\0'; // all strings in C are terminated with a zero (null character); need zero because zeros terminate strings and tells the print ln when to stop.
 return i; // length of the string contained in variable i 
}

int getVersionNumber(char *s)// return length of s
{
 int i; // declare variable i which represents the index into the array; arrays begin at 0 which is why we set i=0
 // Read from device
 Wire.beginTransmission(DEVICE_ADDRESS); // begin transmission at DEVICE_ADDRESS (connect to device 0x05
 Wire.write(REG_VERSION); // write to the internal register you want to read; the sensor version
 Wire.endTransmission(1); // send stop sequence (true(1)); master will release I2C bus

 Wire.requestFrom(DEVICE_ADDRESS, NUM_BYTES); // request 8 bytes from DEVICE_ADDRESS
 while (Wire.available()) // while available,
 {
   s[i++] = Wire.read(); // i++ increments i so we can store the new data in a sequential space on the array
 }
 s[i] = '\0'; // all strings in C are terminated with a zero (null character); need zero because zeros terminate strings and tells the print ln when to stop.
 return i; // length of the string contained in variable i 
}




//----------------------------------------------------------------------------------------------------------------------------------------------------------------
int motor1(int percent)
{
 // if percentage is between -100 and 100
 if (-100 <= percent && percent <= 100) 
 {
 // set motor 1 to percentage value
    Wire.beginTransmission(DEVICE_ADDRESS); //begin transmission at DEVICE_ADDRESS (connect to device 0x05
    Wire.write(M1_POWER); // write to motor 1 power address
    Wire.write(percent); // send the data byte; write the percent specified when function is called to above motor 
    Wire.endTransmission(0); // send a restart message to keep the connection alive (false(0)); master will not release bus
    return 1; // return true (1)
 }
 // else return false (0)
 else
    return 0;
}

int motor2(int percent)
{
 // if percentage is -100<=percent<=100
 if (-100 <= percent && percent <= 100) 
 {
 // then set motor 2 to percentage value
    Wire.beginTransmission(DEVICE_ADDRESS); //begin transmission at DEVICE_ADDRESS (connect to device 0x05)
    Wire.write(M2_POWER); // write to motor 2 power address
    Wire.write(percent); // send data byte; write the percent specified when function is called to the motor above
    Wire.endTransmission(0); // send a restart message to keep the connection alive (false(0)); master will not release bus
    return 1; // return true (1)
 }
 // else return false (0)
 else
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
int motor1and2(int pwr1, int pwr2)
{
 //set motor 1 and motor 2 to move at the same time by setting power
 motor1(pwr1);
 motor2(pwr2);
} 
 
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void stop(void)
{
 //stop motors by setting power to 0
 motor1(0);
 motor2(0);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void turnLeft(int pwr, long t) //t is in miliseconds
{
 //Turn robot left: send right wheel forward and left back
 // write the percent specified when function is called to motor 1 and motor 2
 motor1(-pwr);
 motor2(-pwr);
 delay(t);
 stop();
}

void turnRight(int pwr, long t) //t is in miliseconds
{
 //Turn robot right: send left wheel forward and right back
 // write the percent specified when function is called to motor 1 and motor 2
 motor1(pwr); 
 motor2(pwr);
 delay(t);
 stop();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void straight(int pwr, long t) //t is in miliseconds
{
 motor1and2(pwr, -pwr);
 delay(t);
 stop();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void reverse(int pwr, long t) //t is in miliseconds
{
 motor1and2(-pwr, pwr);
 delay(t);
 stop();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// from this test I found the robot to advance about 9.5 in per second
// the first interval of time (1 second) it advanced 8 in, the next (2.5 sec) 31 in, and the last (5 sec) 41.5 in
// this means it travled a total of 80.5 in in 8.5 sec at motor power 10
// *note that testing was done in a stairwell, the surface had grippers so it was smooth, but a bit rigid
// the average distances traveled per second during each interval were: 8, 12.4, and 8.3; the average of these being 9.56
// the average on the total distance after the total time was 9.47; so the average of the two was about 9.5 in
// this can be used to make a proportion if you want to calculate the exact distance you want the bot to travel
// note the exact distance will vary greatly between surfaces and across speeds, but similar methods can be used to establish this estimate
// 
void straightTest(void)
{
 straight(10, 1000);
 stop();
 delay(2000);
 straight(10, 2500);
 stop();
 delay(2000);
 straight(10, 5000);
 stop();
 delay(1000);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// I was trying to achieve a 90 degree turn
// this was really hard to determine since I'm setting the length the motors run rather than controlling the degree turned
// I noticed that in order to achieve the same turn, the left turn needed a slightly longer run time, 
// perhaps because I was calling it second in the loop, or perhaps because of the way the motors are daisy chained
//
void turnRightTest(void)
{
 turnRight(25, 850);
 stop();
 delay(2000);
 turnRight(25, 875);
 stop();
 delay(2000);
 turnRight(25, 900);
 stop();
 delay(1000);
}

void turnLeftTest(void)
{
 turnLeft(25, 850);
 stop();
 delay(2000);
 turnLeft(25, 875);
 stop();
 delay(2000);
 turnLeft(25, 900);
 stop();
 delay(2000);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void exampleRun(void)
{
 straight(10, 2105); // moves robot forward about 20 in (when power is set to 10%)
 stop();
 turnLeft(25, 875); // when testing turns, running for 8.75 seconds seemed to create a 90 degree turn (left)on the surface tested (when power was set to 25%)
 stop();
 delay(500);
 straight(10, 3158); // moves robot forward about 30 in (when power is set to 10%)
 stop();
 reverse(10, 1053); // reverses robot about 10 in (when power is set to 10%)
 stop();
 turnRight(25, 850); // when testing turns, running for 8.5 seconds seemed to create a 90 degree turn (right) on the surface tested (when power was set to 25%)
 stop();
 straight(10, 4210); // moves robot forward about 40 in (when power is set to 10%)
 stop();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup()
{
 Serial.begin(9600);
 Wire.begin(); // join i2c bus (address optional for master)
}

void loop()
{
 char a[10];
 int length;
 length = getSensorType(a);
 Serial.println(a);
 exampleRun();
 
 while(true); // while loop exits once the condition is false; in this case it never is so we are stuck here and the loop doesn't repeat
}