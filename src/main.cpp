
/*

toyka projekt

Av: Timjan & Noc
2021

*/

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>

#include "test.h"
#include "webCode.h"
#include "MotorController.h"
#include "WebServer.h"
#include "auth.h"


// --- Wifi and webserver stuff ---

#ifndef APSSID
#define APSSID "toyka"
#define APPSK  "password"     // OBS, kan ej vara samma som SSID (iallafall inte som en #defined)
#endif


// --- Defining of objects and variables ---

Adafruit_MPU6050 mpu;

MotorController steeringMotor(200, 13, 12);
MotorController driveMotor(255, 14, 15);

WebServer web(ssid, password, ssidAP, passwordAP);

const int led = 3;


void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("Start of program!");

  web.initWebSocket();
  steeringMotor.init();
  driveMotor.init();
  pinMode(led, OUTPUT);

  // --- Initializing the MPU ---
  
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip? Maybe not...");
  }
  else  
    Serial.println("MPU6050 Found! :D");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  
  delay(100);
}

void loop() {
  
  web.updateWebSocket();
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
  int angle = web.getAngle();
  int speed = web.getSpeed();
  int x = web.getX();
  int y = web.getY();

  
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  

  /* Print out the values */

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

  // --- Testing code for motors ----------- 


  int motorSpeed = map(abs(y), 0, 200, 0, 100);

  if (y < 0) {
    driveMotor.drive(MotorController::forward, motorSpeed);
  }
  else if (y > 0) {
    driveMotor.drive(MotorController::reverse, motorSpeed);
  }
  else {
    driveMotor.drive(MotorController::neutral, 0);
  }

  // LED stuff
  if (y > 10)
    digitalWrite(led, HIGH);
  else
    digitalWrite(led, LOW);

  // Steering stuff
  if (x < -50) 
    steeringMotor.steer(MotorController::left);
  else if (x > 50) 
    steeringMotor.steer(MotorController::right);
  else 
    steeringMotor.steer(MotorController::forward);

/*
  analogWrite(12, 127);
  digitalWrite(13, LOW);
  //steeringMotor.drive(MotorController::forward, 100);
  delay(2000);
  digitalWrite(13, HIGH);
  //steeringMotor.drive(MotorController::reverse, 100);
  delay(2000);
*/


  /*if (Serial.available()) {
    char serialSignal = Serial.read();
    Serial.println(serialSignal);

    if (serialSignal == 'l')
      steeringMotor.steer(MotorController::direction::left);
    else if (serialSignal == 'r')
      steeringMotor.steer(MotorController::direction::right);
    else if (serialSignal == 'f')
      steeringMotor.steer(MotorController::direction::forward);
  }*/

  

}