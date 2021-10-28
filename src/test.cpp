
#include <Arduino.h>
#include "test.h"

void testFuncUniqueToNoc() {
    pinMode(2, OUTPUT);
    while(1) {
        digitalWrite(2, LOW);

        delay(1000);

        digitalWrite(2, HIGH);

        delay(1000);
    }
    
}