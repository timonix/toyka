#include <Arduino.h>
#include "SteeringController.h"


SteeringController::SteeringController(Adafruit_MPU6050 mpuUnit) {
    
    this->mpu = mpuUnit;

}


void SteeringController::init() {
    
    // --- Initializing the MPU ---
    
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip? Maybe not...");
    }
    else  
        Serial.println("MPU6050 Found! :D");

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    //zGyroBias = calculateZGyroBias();     // calculate the Z gyro bias, in order to calibrate
}


void SteeringController::readMPUValues() {
    mpu.getEvent(&a, &g, &temp);
    zGyro = g.gyro.z - zGyroBias;
}


float SteeringController::calculateZGyroBias() {
    
    zGyroBias = g.gyro.z;

    return 0;

    /*
    Serial.print("Starting calibration of gyro!");

    float sum = 0;

    while (true) {
        readMPUValues();

        Serial.print("acc: ");
        Serial.println(a.acceleration.z);

        if (a.acceleration.z > 9)
            break;
    }

    delay(3000);

    for (int i = 0; i < 100; i++) {
        readMPUValues();
        sum += g.gyro.z;
        delay(10);
    }

    return sum / 100;   // return the average value of the 100 samples

    Serial.println("Calibrated!");
    */
}


float SteeringController::PIDController(float input, float measuredValue) {      // setPoint in -200 to 200, measuredValue in rad/s

    // Converting the input setPoint to rad/s
    input = -1 * input;         // inverting the signal to match the turning
    float setPoint = maxAngleSpeed * (input / 200);

    //float setPoint = 1.0;
    unsigned int currentTime = millis();
    float deltaTime = (currentTime - lastTime);      // Time difference in milliseconds
    lastTime = currentTime;

    float error = setPoint - measuredValue;

    P = Kp * error;
    
    if (!(abs(PIDControlSignal) >= maxSignalAbs)) {
        cumError += error * deltaTime / 1000;
        I = Kp * (1 / Ti) * cumError;
    }

    D = Kp * Td * ((measuredValue - lastMeasuredValue) / deltaTime);
    
    PIDControlSignal = P + I + D;
    /*
    Serial.print(setPoint);
    Serial.print(" P: ");
    Serial.print(P);
    Serial.print(" I: ");
    Serial.print(I);
    Serial.print(" D: ");
    Serial.print(D);
    Serial.print(" ");
    */
    lastMeasuredValue = measuredValue;

    
    if (PIDControlSignal > maxSignalAbs)
        return maxSignalAbs;
    else if (PIDControlSignal < -maxSignalAbs)
        return -maxSignalAbs;
    else
        return PIDControlSignal;
}

/*
float SteeringController::calculateZGyroBias() {
  Serial.print("Starting calibration of gyro!");
  int startTime = millis();
  int elapsedTime = startTime;
  int targetTime = 1000;

  float lastValue = 0;
  float measuredValue = 0;

  while (true) {
    //sensors_event_t a, g, temp;
    //SteeringController::mpu.getEvent(&a, &g, &temp);
    readMPUValues();

    measuredValue = float(int(g.gyro.z * 100)) / 100;   // Rounding the value to two decimals


    Serial.print("Measured value: ");
    Serial.print(measuredValue);
    Serial.print("  Elapsed time: ");
    Serial.print(elapsedTime);
    Serial.print("  Time Difference: ");
    Serial.println(elapsedTime - startTime);

    
    if (measuredValue != lastValue)               // Restarting test if values change
      startTime = elapsedTime;
    
    if ((elapsedTime - startTime) > targetTime)   // Return if target time is reached without interruption
      return measuredValue;

    lastValue = measuredValue;
    elapsedTime = millis();
  }

  Serial.println("Calibrated!");
}
*/