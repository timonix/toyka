

#include <Arduino.h>
#include "MotorController.h"


MotorController::MotorController(int maxMotorSpeed, int motorPinA, int motorPinB)
{
    this->maxMotorSpeed = maxMotorSpeed;
    this->motorPinA = motorPinA;
    this->motorPinB = motorPinB;
    this->motorSpeedDeltaTime = 0;
    this->motorSpeedLastTime = 0;

    this->lastDirection = MotorController::direction::forward;

}



void MotorController::init() {
    pinMode(this->motorPinA, OUTPUT);
    pinMode(this->motorPinB, OUTPUT);

    digitalWrite(this->motorPinA, LOW);
    digitalWrite(this->motorPinB, LOW);

    /*
    this->steer(MotorController::direction::left);         // Turn left and then forward to reset the front wheels. OBS not used, since this class is for both steering and driving of the car
    delay(1000);
    this->steer(MotorController::direction::forward);
    */
}



static inline int asm_ccount(void) {

    int r;

    asm volatile ("rsr %0, ccount" : "=r"(r));
    return r;
}

void MotorController::controlMotorSpeed(int pwmPin, int targetMotorSpeed) {
    
    int currentTime = asm_ccount();
    this->motorSpeedDeltaTime = currentTime - this->motorSpeedLastTime;

    analogWrite(pwmPin, targetMotorSpeed);

/*
    if (MotorController::motorSpeedDeltaTime >= (MotorController::motorSpeedAccelerationTime * 80000)) {

        this->motorSpeed += MotorController::motorSpeedAccelerationValue;

        if (this->motorSpeed > targetMotorSpeed) {
            this->motorSpeed = targetMotorSpeed;
        }

        Serial.println(this->motorSpeed);

        analogWrite(pwmPin, MotorController::motorSpeed);

        this->motorSpeedLastTime = asm_ccount();
    }
    */
}


void MotorController::autoSteer(float controlSignal, direction direction) {

    float motorSignal;

    if (direction == MotorController::forward) {

        if (controlSignal < 0) {
            motorSignal = map(controlSignal, 0, -100, minSteerSpeed, maxSteerSpeed);

            digitalWrite(motorPinA, LOW);
            analogWrite(motorPinB, motorSignal);
        }
        else if (controlSignal > 0) {
            motorSignal = map(controlSignal, 0, 100, minSteerSpeed, maxSteerSpeed);

            digitalWrite(motorPinB, LOW);
            analogWrite(motorPinA, motorSignal);
        }
        else if (controlSignal == 0) {
            digitalWrite(motorPinA, LOW);
            digitalWrite(motorPinB, LOW);
        }
    }
    else if (direction == MotorController::reverse) {
        
        if (controlSignal < 0) {
            motorSignal = map(controlSignal, 0, -100, minSteerSpeed, maxSteerSpeed);

            digitalWrite(motorPinB, LOW);
            analogWrite(motorPinA, motorSignal);
        }
        else if (controlSignal > 0) {
            motorSignal = map(controlSignal, 0, 100, minSteerSpeed, maxSteerSpeed);

            digitalWrite(motorPinA, LOW);
            analogWrite(motorPinB, motorSignal);
        }
        else if (controlSignal == 0) {
            digitalWrite(motorPinA, LOW);
            digitalWrite(motorPinB, LOW);
        }
    }
    else if (direction == MotorController::neutral) {
        digitalWrite(motorPinA, LOW);
        digitalWrite(motorPinB, LOW);
    }
    return;
}


void MotorController::steer(MotorController::direction direction) {

    if (direction == MotorController::direction::forward && this->lastDirection == MotorController::direction::left) {           // The vehicle stops turning left and starts driving straight forward
        
        //Serial.println("Turning forward!");
        
        analogWrite(this->motorPinB, maxMotorSpeed);
        digitalWrite(this->motorPinA, LOW);
        delay(this->steeringReturnTime);
        digitalWrite(this->motorPinB, LOW);
    }
    else if (direction == MotorController::direction::forward && this->lastDirection == MotorController::direction::right) {     // The vehicle stops turning right and starts driving straight forward
        
        //Serial.println("Turning forward!");
        
        analogWrite(this->motorPinA, maxMotorSpeed);
        digitalWrite(this->motorPinB, LOW);
        delay(this->steeringReturnTime);
        digitalWrite(this->motorPinA, LOW);
    }
    else if (direction == MotorController::direction::left && this->lastDirection != MotorController::direction::left) {         // Turn the vehicle left if its the first time "left"-variable is activated
        
        //Serial.println("Turning left!");
        
        analogWrite(this->motorPinA, this->maxMotorSpeed);
        digitalWrite(this->motorPinB, LOW);
        delay(this->steeringTurnTime);

        digitalWrite(this->motorPinA, LOW);
    }
    else if (direction == MotorController::direction::right && this->lastDirection != MotorController::direction::right) {        // Turn the vehicle right if its the first time "right"-variable is activated
        
        //Serial.println("Turning Right!");
        
        analogWrite(this->motorPinB, this->maxMotorSpeed);
        digitalWrite(this->motorPinA, LOW);
        delay(this->steeringTurnTime);

        digitalWrite(this->motorPinB, LOW);
    }

    this->lastDirection = direction;

    return;
}



void MotorController::drive(MotorController::direction direction) {                            // Drive Motor forward and backward, OBS! NOT TESTED
    
    if (direction == MotorController::direction::neutral) {
        digitalWrite(this->motorPinA, LOW);
        digitalWrite(this->motorPinB, LOW);
    }
    else if (direction == MotorController::direction::forward) {
        MotorController::controlMotorSpeed(MotorController::motorPinA, MotorController::maxMotorSpeed);
        digitalWrite(this->motorPinB, LOW);
    }
    else if (direction == MotorController::direction::reverse) {
        digitalWrite(this->motorPinA, LOW);
        MotorController::controlMotorSpeed(MotorController::motorPinB, MotorController::maxMotorSpeed);
    }
}



void MotorController::drive(MotorController::direction direction, float speedPercent) {         // Drive Motor forward and backward, OBS! NOT TESTED     

    // Speedpercent should be between 0 and 100

    this->targetMotorSpeed = int(speedPercent/100 * float(this->maxMotorSpeed));

    if (direction == MotorController::direction::neutral) {
        digitalWrite(this->motorPinA, LOW);
        digitalWrite(this->motorPinB, LOW);
    }
    else if (direction == MotorController::direction::forward) {
        digitalWrite(this->motorPinB, LOW);
        MotorController::controlMotorSpeed(MotorController::motorPinA, this->targetMotorSpeed);
    }
    else if (direction == MotorController::direction::reverse) {
        digitalWrite(this->motorPinA, LOW);
        MotorController::controlMotorSpeed(MotorController::motorPinB, this->targetMotorSpeed);
    }
}