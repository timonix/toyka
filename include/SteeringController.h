#ifndef MPUCONTROLLER_H
#define MPUCONTROLLER_H

#include <Adafruit_MPU6050.h>

class SteeringController
{

public:

    SteeringController(Adafruit_MPU6050 mpuUnit);
    void init();
    void readMPUValues();
    float calculateZGyroBias();
    float PIDController(float input, float measuredValue);
    Adafruit_MPU6050 mpu;
    sensors_event_t a, g, temp;     // Raw values from MPU
    float zGyro;                    // Calibrated gyro value

private:

    float zGyroBias = 0;

    float maxAngleSpeed = 1.5;

    // ----- PID stuff here -----

    float maxSignalAbs = 100;           // max +- x as a control signal

    float Kp = 20;
    float Ti = 5000;
    float Td = 5;

    /*
    Kp = 20, Ti = 0.2, Td = 0       Ger helt ok styrning, om lite vingligt
    */

    float P = 0;
    float I = 0;
    float D = 0;
    float PIDControlSignal = 0;

    float cumError = 0;
    float lastMeasuredValue = 0;

    unsigned int lastTime = 0;
};

#endif