#include "PS4ControllerHandler.h"
#include "MotorControl.h"

extern bool frontIsReversed;
extern bool isPrecisionMode;

int forwardSetpoint = 0;
int sidewaysSetpoint = 0;
int turnSetpoint = 0;

void checkPS4(int speed) {
    const int deadband = 5;

    if (PS4.isConnected()) {
        if (PS4.Up()) {
            frontIsReversed = false;
        }
        if (PS4.Down()) {
            frontIsReversed = true;
        }
        isPrecisionMode = PS4.Right();

        int rawY = PS4.LStickY();
        int rawX = PS4.LStickX();
        int rawTurn = PS4.RStickX();
        
        forwardSetpoint = (abs(rawY) < deadband) ? 0 : map(rawY, -127, 127, -speed, speed);
        sidewaysSetpoint = (abs(rawX) < deadband) ? 0 : map(rawX, -127, 127, speed, -speed);
        turnSetpoint = (abs(rawTurn) < deadband) ? 0 : map(rawTurn, -127, 127, -speed, speed);
    }
}