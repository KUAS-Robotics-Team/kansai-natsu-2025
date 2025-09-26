#include "IntakeControl.h"
#include "PS4ControllerHandler.h"
#include "CANBusHandler.h"
extern bool frontIsReversed;


void controlIntake()
{
    short intakeSpeed = 0;
    const short intakeMaxSpeed = 5000; 
    if (PS4.isConnected())
    {
        if (frontIsReversed){
        if (PS4.R2())
        {
            intakeSpeed = -intakeMaxSpeed;
        }
        else if (PS4.R1())
        {
            intakeSpeed = intakeMaxSpeed;
        }
        else
        {
            intakeSpeed = 0;
        }
    }
    }

    byte data[8] = {
        intakeSpeed >> 8, intakeSpeed,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00};

    sendCANMessage(0x1FF, data, 8);
}
