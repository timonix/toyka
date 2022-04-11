
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
#include "UdpTransceiver.h"
#include "MotorController.h"
//#include "WebServer.h"
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

//WebServer web(ssid, password, ssidAP, passwordAP);
UdpTransceiver tcp(ssid, password, ssidAP, passwordAP);

const int led = 3;
const int analogInPin = A0;


void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("Start of program!");
  tcp.initServer();

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
  delay(10);
  tcp.update();
  uint8_t packet[255];
  tcp.receiveData(packet);


  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  tcp.publishSensor(GYROSCOPE_HEADER,g.data,micros());

}