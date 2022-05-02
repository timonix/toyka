
/*

toyka projekt

Av: Timjan & Noc
2021

*/

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <math.h>

#include "webCode.h"
#include "MotorController.h"
#include "SteeringController.h"
#include "auth.h"
#include "UdpTransceiver.h"


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

UdpTransceiver tcp(ssid, password, ssidAP, passwordAP);

const int led = 3;
const int analogInPin = A0;

const int buzzer = 2;

int driveSpeed = 0;
int targetDirection = 0;


void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("Start of program!");
  tcp.initServer();

  //web.initWebSocket();
  steeringMotor.init();
  driveMotor.init();
  steeringController.init();
  pinMode(led, OUTPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
 
  delay(100);
}


  void honk(bool honkOn) {
    if (honkOn) {
      analogWriteFreq(800);
      analogWrite(buzzer, 500);
    }
    else
      analogWrite(buzzer, 0);
  }


void loop() {
  
  tcp.update();

  // --- Fetching and calculating signals ---------------------------------
  
  int8_t packet[255];
  tcp.receiveData(packet);

  if (packet[0] == TARGET_SPEED_HEADER) {
    if (packet[1] != driveSpeed){
      driveSpeed = packet[1];
      Serial.print("new speed:");
      Serial.println(driveSpeed);
    }
  }

  if (packet[0] == STICK_DIRECTION_HEADER) {
    if (packet[1] != targetDirection){
      targetDirection = packet[1];
      Serial.print("new direction:");
      Serial.println(targetDirection);
    }
  }
  
  steeringController.readMPUValues();
  float z = steeringController.zGyro;
  float steeringControlSignal = steeringController.PIDController(float(targetDirection), z);


  // --- Code for motors --------------------------------------------------- 


  if (driveSpeed > 0) {
    driveMotor.drive(MotorController::forward, driveSpeed);
    steeringMotor.autoSteer(steeringControlSignal, MotorController::forward);
  }
  else if (driveSpeed < 0) {
    driveMotor.drive(MotorController::reverse, driveSpeed);
    steeringMotor.autoSteer(steeringControlSignal, MotorController::reverse);
  }
  else {
    driveMotor.drive(MotorController::neutral, 0);
    steeringMotor.autoSteer(steeringControlSignal, MotorController::neutral);
    steeringController.calculateZGyroBias();
  }


  // --- LED and other stuff -------------------------------------

  if (driveSpeed < -10)
    digitalWrite(led, HIGH);
  else
    digitalWrite(led, LOW);


  delay(0);
}