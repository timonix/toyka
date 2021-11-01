

#include <Arduino.h>
#include "MotorController.h"


MotorController::MotorController(int maxMotorSpeed, int motorPinA, int motorPinB)
{
    this->maxMotorSpeed = maxMotorSpeed;
    this->motorPinA = motorPinA;
    this->motorPinB = motorPinB;

    this->lastDirection = MotorController::direction::forward;

}

void MotorController::init() {
    pinMode(this->motorPinA, OUTPUT);
    pinMode(this->motorPinB, OUTPUT);

    this->steer(MotorController::direction::left);         // Turn left and then forward to reset the front wheels
    delay(1000);
    this->steer(MotorController::direction::forward);
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
        digitalWrite(this->motorPinB, LOW);
        analogWrite(this->motorPinA, maxMotorSpeed);
    }
    else if (direction == MotorController::direction::reverse) {
        digitalWrite(this->motorPinA, LOW);
        analogWrite(this->motorPinB, maxMotorSpeed);
    }
}

void MotorController::drive(MotorController::direction direction, float speedPercent) {         // Drive Motor forward and backward, OBS! NOT TESTED
    
    this->targetMotorSpeed = int(speedPercent * float(this->maxMotorSpeed));

    if (direction == MotorController::direction::neutral) {
        digitalWrite(this->motorPinA, LOW);
        digitalWrite(this->motorPinB, LOW);
    }
    else if (direction == MotorController::direction::forward) {
        digitalWrite(this->motorPinB, LOW);

        /*                                                                              // TODO - Make a new private function to ramp the speed up/down?
        while (this->motorSpeed < this->targetMotorSpeed) {
            this->motorSpeed += 1;
            analogWrite(this->motorPinA, this->motorSpeed);
            delay(5);
        }
        */

        analogWrite(this->motorPinA, maxMotorSpeed);
    }
    else if (direction == MotorController::direction::reverse) {
        digitalWrite(this->motorPinA, LOW);
        analogWrite(this->motorPinB, maxMotorSpeed);
    }

    this->lastMotorSpeed = this->motorSpeed;
}