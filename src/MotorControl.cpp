#include "MotorControl.h"
#include "PS4ControllerHandler.h"

extern bool frontIsReversed;
extern bool isPrecisionMode;


void driveWithPid(int forwardSetpoint, int sidewaysSetpoint, int turnSetpoint)
{
    updateMotorRPM();
    float setpointM1, setpointM2, setpointM3, setpointM4;

    if (frontIsReversed)
    {
        setpointM1 = -forwardSetpoint - sidewaysSetpoint + turnSetpoint;
        setpointM2 = -forwardSetpoint + sidewaysSetpoint + turnSetpoint;
        setpointM3 = forwardSetpoint + sidewaysSetpoint + turnSetpoint;
        setpointM4 = forwardSetpoint - sidewaysSetpoint + turnSetpoint;
    }
    else
    {
        setpointM1 = forwardSetpoint - sidewaysSetpoint + turnSetpoint;
        setpointM2 = -forwardSetpoint - sidewaysSetpoint + turnSetpoint;
        setpointM3 = -forwardSetpoint + sidewaysSetpoint + turnSetpoint;
        setpointM4 = forwardSetpoint + sidewaysSetpoint + turnSetpoint;
    }

    float controlM1 = computePid(setpointM1, motorRPMData.M1);
    float controlM2 = computePid(setpointM2, motorRPMData.M2);
    float controlM3 = computePid(setpointM3, motorRPMData.M3);
    float controlM4 = computePid(setpointM4, motorRPMData.M4);

    short speedM1 = (short)controlM1;
    short speedM2 = (short)controlM2;
    short speedM3 = (short)controlM3;
    short speedM4 = (short)controlM4;

    if (isPrecisionMode)
    {
        speedM1 *= 0.5;
        speedM2 *= 0.5;
        speedM3 *= 0.5;
        speedM4 *= 0.5;
    }

    byte data[8] = {speedM1 >> 8, speedM1, speedM2 >> 8, speedM2, speedM3 >> 8, speedM3, speedM4 >> 8, speedM4};

    sendCANMessage(0x200, data, 8);
}