
// NextGen Head Kit Main Program V1.0
// ATMega328P
// Select Arduino Uno in the IDE

//##################################################################
//Include Libraries
#include <Wire.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <DMXSerial.h>

//##################################################################
//Time keeping
unsigned long currentMillis;

long previousMillis = 0;    // set up timers

int loopTime;
int previousLooptime;

int LEDprev;
int PIXELprev;

//##################################################################
//i2C PWM Driver
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver head = Adafruit_PWMServoDriver(0x40);


//##################################################################
//PWM Driver Settings
#define SERVOMIN  67 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  527 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

//##################################################################
//Serial Integers
int ard = 0;
int input = 0;
int pulselength = 0;

int neckRotate = 1000;
int neckLeft = 1750;
int neckRight = 1250;

int mouthRight = 1500;
int mouthLeft = 1150;

int eyeLeftV = 1500;
int eyeLeftH = 1500;
int eyeRightV = 1500;
int eyeRightH = 1500;


//##################################################################
//Smoothing Variables
float neckRotateSmoothed = 1000;
float neckRotatePrev;// = 1000;
float neckLeftSmoothed = 1750;
float neckLeftPrev;// = 1750;
float neckRightSmoothed = 1250;
float neckRightPrev;// = 1250;

float mouthLeftSmoothed = 1500;
float mouthLeftPrev;// = 1500;
float mouthRightSmoothed = 1150;
float mouthRightPrev;// = 1150;

float eyeLeftVSmoothed = 1500;
float eyeLeftVPrev;// = 1500;
float eyeLeftHSmoothed = 1150;
float eyeLeftHPrev;// = 1150;

float eyeRightVSmoothed = 1500;
float eyeRightVPrev;// = 1500;
float eyeRightHSmoothed = 1150;
float eyeRightHPrev;// = 1150;


//##################################################################
//Lower positions (starting numbers)
int neckRotateLower = 1000;
int neckLeftLower = 1750;
int neckRightLower = 1250;

int mouthLeftLower = 1500;
int mouthRightLower = 1150;


//##################################################################
//DMX Set Up
uint8_t ch1;    uint8_t ch2;    uint8_t ch3;
uint8_t ch4;    uint8_t ch5;    uint8_t ch6;
uint8_t ch7;    uint8_t ch8;    uint8_t ch9;
uint8_t ch10;   uint8_t ch11;   uint8_t ch12;
 

uint8_t lastValue = 0;

//##################################################################
//Face tracking variables
int sw1 = 2;
int neckH;
int neckV;

int neckVScale;
int neckHScaleR;
int neckHScaleL;

//##################################################################
//Serial variables
int dat = 0;

//##################################################################
//Setup Function
void setup() {

  //#########################
  //Start Head
  head.begin();
  head.setOscillatorFrequency(27000000);
  head.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  //#########################
  //Indicator LED
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  //#########################
  //Start values
  head.writeMicroseconds(0, 1000);
  head.writeMicroseconds(1, 1750);
  head.writeMicroseconds(2, 1250);
  head.writeMicroseconds(3, 1500);
  head.writeMicroseconds(4, 1150);

  head.writeMicroseconds(5, 1500);
  head.writeMicroseconds(6, 1500);
  head.writeMicroseconds(7, 1500);
  head.writeMicroseconds(8, 1500);
  
  delay(10);

  //#########################
  //Start DMX
  DMXSerial.init(DMXReceiver);

  
}





//##################################################################
//Main program loop
void loop() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= 10) {  // start timed event
    previousMillis = currentMillis; 

    //################################################
    //LED indicator flashing
    if (currentMillis - LEDprev >= 500) {
      digitalWrite(13, !digitalRead(13));
      if (digitalRead(13) == 1)
      {
        head.writeMicroseconds(15, 4096);
      }
      else
      {
        head.writeMicroseconds(15, 0);
      }
      LEDprev = currentMillis;
    }
    
      //################################################
      //Get DMX Values
      ch1 = DMXSerial.read(1); //Neck Rotate
      ch2 = DMXSerial.read(2); //Neck Left
      ch3 = DMXSerial.read(3); //Neck Right
      ch4 = DMXSerial.read(4); //Mouth open/close

      ch5 = DMXSerial.read(5); //Left Eye Up/Down
      ch6 = DMXSerial.read(6); //Left Eye Right/Left
      ch7 = DMXSerial.read(7); //Right Eye Up/Down
      ch8 = DMXSerial.read(8); //Right Eye Right/Left
   

      //################################################
      //Assign and map
      neckRotate = map(ch1, 0, 255, 1000, 2000);
      neckLeft = map(ch2, 0, 255, 1750, 1250);
      neckRight = map(ch3, 0, 255, 1250, 1750);

      mouthLeft = map(ch4, 0, 255, 1250, 1500);
      mouthRight = map(ch4, 0, 255, 1450, 1150);

      eyeLeftV = map(ch5, 0, 255, 1150, 1850);
      eyeLeftH = map(ch6, 0, 255, 1150, 1850);
      eyeRightV = map(ch7, 0, 255, 1150, 1850);
      eyeRightH = map(ch8, 0, 255, 1150, 1850);

      
      //################################################
      //Smoothing
      neckRotateSmoothed = (neckRotate * 0.10) + (neckRotatePrev * 0.90);
      neckRotatePrev = neckRotateSmoothed;
    
      neckLeftSmoothed = (neckLeft * 0.10) + (neckLeftPrev * 0.90);
      neckLeftPrev = neckLeftSmoothed;
    
      neckRightSmoothed = (neckRight * 0.10) + (neckRightPrev * 0.90);
      neckRightPrev = neckRightSmoothed;
    

      mouthLeftSmoothed = (mouthLeft * 0.20) + (mouthLeftPrev * 0.80);
      mouthLeftPrev = mouthLeftSmoothed;
      mouthRightSmoothed = (mouthRight * 0.20) + (mouthRightPrev * 0.80);
      mouthRightPrev = mouthRightSmoothed;

      eyeLeftVSmoothed = (eyeLeftV * 0.02) + (eyeLeftVPrev * 0.98);
      eyeLeftVPrev = eyeLeftVSmoothed;
      eyeLeftHSmoothed = (eyeLeftH * 0.02) + (eyeLeftHPrev * 0.98);
      eyeLeftHPrev = eyeLeftHSmoothed;

      eyeRightVSmoothed = (eyeRightV * 0.02) + (eyeRightVPrev * 0.98);
      eyeRightVPrev = eyeRightVSmoothed;
      eyeRightHSmoothed = (eyeRightH * 0.02) + (eyeRightHPrev * 0.98);
      eyeRightHPrev = eyeRightHSmoothed;


      //################################################
      //Wite values
      head.writeMicroseconds(0, neckRotateSmoothed);
      head.writeMicroseconds(1, neckLeftSmoothed);
      head.writeMicroseconds(2, neckRightSmoothed);

      head.writeMicroseconds(3, mouthLeft);
      head.writeMicroseconds(4, mouthRight);

      head.writeMicroseconds(5, eyeLeftVSmoothed);
      head.writeMicroseconds(6, eyeLeftHSmoothed);
      head.writeMicroseconds(7, eyeRightVSmoothed);
      head.writeMicroseconds(8, eyeRightHSmoothed);

  }// end of timed event
}






/*
 * PCA Board Pinouts:
 * 
 * Head:
 * 0:   Neck Rotate
 * 1:   Neck Left
 * 2:   Neck Right
 * 3:   Jaw Left
 * 4:   Jaw Right
 * 5:   Left Eye Up/Down
 * 6:   Left Eye Right/Left
 * 7:   Right Eye Up/Down
 * 8:   Right Eye Right/Left
 * 9:   Left Eyelid top
 * 10:  Left Eyelid bottom
 * 11:  Right Eyelid top
 * 12:  Right Eyelid bottom
 * 
 */
