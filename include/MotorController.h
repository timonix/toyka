
#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H


class MotorController
{
    
public:

    enum direction {                // Directions used in MotorController class
        forward,
        right,
        left,
        reverse,
        neutral
    };
    
    MotorController(int maxMotorSpeed, int motorPinA, int motorPinB);
    void init();
    void steer(direction direction);
    void steer(direction direction, float speedPercent);       // speedPercent is a float value between 0-1

    void drive(direction direction);
    void drive(direction direction, float speedPercent);       // speedPercent is a float value between 0-1

private:
    
    int maxMotorSpeed;               // max motor speed in PWM 0-1023, used when no specific speed is choosed
    int motorSpeed;                  // Motor speed in PWM 0-1023. Actual motor speed
    int targetMotorSpeed;            // wanted motor speed in PWM 0-1023. Used when calculating a specific wanted speed

    // Arbitrary pin A & B. Change in real life or in class call/contruction if wrong direction
    int motorPinA;                   // Hardware OUPUT Pin A
    int motorPinB;                   // Hardware OUPUT Pin B

    int steeringTurnTime = 200;      // Time of signal to motors when turning left or right
    int steeringReturnTime = 20;     // Time of signal to motors when turning forward

    direction lastDirection;
    int lastMotorSpeed;              // Used in ramping of speed and such??

};




#endif