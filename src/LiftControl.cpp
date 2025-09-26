#include "LiftControl.h"
#include "PS4ControllerHandler.h"
#include "CANBusHandler.h"

extern bool frontIsReversed;

void controlLift() {
    short liftSpeed      = 0;
    const short maxSpeed = 16000;

    if (PS4.isConnected()) {
        if (!frontIsReversed){
        if (PS4.L2())      liftSpeed =  maxSpeed;
        else if (PS4.L1()) liftSpeed = -maxSpeed;
        else                   liftSpeed =  0;
        }
    }

    byte data[8] = {
        0x00, 0x00,
        0x00, 0x00,
        static_cast<byte>(liftSpeed >> 8),
        static_cast<byte>(liftSpeed),
        0x00, 0x00
    };

    sendCANMessage(0x1FF, data, 8);
}
