
/*

toyka projekt

Av: Timjan & Noc
2021

*/

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "test.h"
#include "webCode.h"
#include "MotorController.h"


// --- Wifi and webserver stuff ---

#ifndef APSSID
#define APSSID "toyka"
#define APPSK  "password"     // OBS, kan ej vara samma som SSID (iallafall inte som en #defined)
#endif

const char *ssid = APSSID;
const char *password = APPSK;

IPAddress apIP(192, 168, 1, 1);


// --- Defining of objects and variables ---

Adafruit_MPU6050 mpu;
ESP8266WebServer server(80);

MotorController steeringMotor(300, 14, 15);
MotorController driveMotor(100, 12, 13);

void webTest() {
  server.send(200, "text/html", "<h1>Du är online, kompis</h1>");
}


void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("Start of program!");

  steeringMotor.init();
  driveMotor.init();

  // --- Initializing the MPU ---
  
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip? Maybe not...");
  }
  else  
    Serial.println("MPU6050 Found! :D");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  

  // --- Webserver stuff ---

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); 
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(apIP);

  server.on("/test", webTest);
  server.begin();
  Serial.print("Server started");


  
  delay(100);
}

void loop() {
  
  server.handleClient();


  
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
  delay(50);
  

  // --- Testing code for motors -----------

  if (Serial.available()) {
    char serialSignal = Serial.read();
    Serial.println(serialSignal);

    if (serialSignal == 'l')
      steeringMotor.steer(MotorController::direction::left);
    else if (serialSignal == 'r')
      steeringMotor.steer(MotorController::direction::right);
    else if (serialSignal == 'f')
      steeringMotor.steer(MotorController::direction::forward);
  
    if (serialSignal == '1')
      driveMotor.drive(MotorController::direction::forward);
    else if (serialSignal == '2')
      driveMotor.drive(MotorController::direction::neutral);
    else if (serialSignal == '3') 
      driveMotor.drive(MotorController::direction::reverse);
  
  }

  

}