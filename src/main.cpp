
/*

toyka projekt

Av: Timjan & Noc
2021

*/

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <math.h>

#include "test.h"
#include "webCode.h"
#include "UdpTransceiver.h"
#include "MotorController.h"
<<<<<<< HEAD
//#include "WebServer.h"
=======
#include "SteeringController.h"
#include "WebServer.h"
>>>>>>> 5b07937 (A lot things :O A bunch of motor and PID stuff)
#include "auth.h"


// --- Wifi and webserver stuff ---

#ifndef APSSID
#define APSSID "toyka"
#define APPSK  "password"     // OBS, kan ej vara samma som SSID (iallafall inte som en #defined)
#endif


// --- Defining of objects and variables ---

Adafruit_MPU6050 mpuUnit;

MotorController steeringMotor(200, 13, 12);
MotorController driveMotor(255, 14, 15);
SteeringController steeringController(mpuUnit);

//WebServer web(ssid, password, ssidAP, passwordAP);
UdpTransceiver tcp(ssid, password, ssidAP, passwordAP);

const int led = 3;
const int analogInPin = A0;

const int buzzer = 2;


//float calculateZGyroBias();


void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("Start of program!");
  tcp.initServer();

  steeringMotor.init();
  driveMotor.init();
  steeringController.init();
  pinMode(led, OUTPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
 
  delay(100);
}


  void customTone(uint8_t _pin, unsigned int frequency, unsigned long duration) {
    //pinMode (_pin, OUTPUT );
    analogWriteFreq(frequency);
    analogWrite(_pin, 50);
    delay(duration);
    analogWrite(_pin,0);
  }

  void engineSound(unsigned int motorSpeed) {
    int maxFrequency = 400;
    int minFrequency = 30;

    int PWM = 150;

    int frequency = map(motorSpeed, 0, 100, minFrequency, maxFrequency);

    analogWriteFreq(frequency);
    analogWrite(buzzer, PWM);

  }

  void honk(bool honkOn) {
    if (honkOn) {
      analogWriteFreq(800);
      analogWrite(buzzer, 500);
    }
    else
      analogWrite(buzzer, 0);
  }
/*
  void timjanTone (uint8_t pin, unsigned int freq, unsigned int duration) {
    unsigned int startTime = micros();
    while(true) {
      analogWrite(pin, sin(micros()*freq*1000000)*255);
      if (micros() > startTime + duration) {
        analogWrite(pin, 0);
        break;
      }
    }
    
  }
*/
void loop() {
<<<<<<< HEAD
  delay(10);
  tcp.update();
  uint8_t packet[255];
  tcp.receiveData(packet);


  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  tcp.publishSensor(GYROSCOPE_HEADER,g.data,micros());
=======
  
  web.updateWebSocket();
 

 // --- Fetching and calculating signals ---------------------------------
  
  int x = web.getX();
  int y = web.getY();
  
  steeringController.readMPUValues();
  float z = steeringController.zGyro;
  float controlSignal = steeringController.PIDController(float(x), z);


  // --- Code for motors --------------------------------------------------- 

  int motorSpeed = map(abs(y), 0, 200, 0, 100);

  if (y < 0) {
    driveMotor.drive(MotorController::forward, motorSpeed);
    steeringMotor.autoSteer(controlSignal, MotorController::forward);
  }
  else if (y > 0) {
    driveMotor.drive(MotorController::reverse, motorSpeed);
    steeringMotor.autoSteer(controlSignal, MotorController::reverse);
  }
  else {
    driveMotor.drive(MotorController::neutral, 0);
    steeringMotor.autoSteer(controlSignal, MotorController::neutral);
    steeringController.calculateZGyroBias();
  }


  // --- LED and other stuff -------------------------------------

  if (y > 10)
    digitalWrite(led, HIGH);
  else
    digitalWrite(led, LOW);

  //engineSound(motorSpeed);

  if (motorSpeed != 0) 
    honk(true);
  
  else
    honk(false);
  
/*
   for (int i = 50; i < 200; i++) {
     //tone(buzzer, i);
     customTone(buzzer, i, 50);
     //timjanTone(buzzer, 400, 50);
     //delay(50);
   }
   delay(1000);
   for (int i = 200; i > 50; i--) {
     //tone(buzzer, i);
     customTone(buzzer, i, 50);
     //delay(50);
   }
*/
  // --- Debug shit ------------------------------------------------
/*
  Serial.print("Z: ");
  Serial.print(abs(z));
  Serial.print("  PID: ");
  Serial.print(controlSignal);
*/
  Serial.println(steeringController.zGyro);
  //Serial.println("");

  /*
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.print(" m/s^2    ");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");
*/
>>>>>>> 5b07937 (A lot things :O A bunch of motor and PID stuff)

  /*
  Serial.print("Angle:");
  Serial.print(web.getAngle());
  Serial.print("   Speed:");
  Serial.print(web.getSpeed());
  Serial.print("   x:");
  Serial.print(web.getX());
  Serial.print("   y:");
  Serial.println(web.getY());
*/
 
  delay(20);
}