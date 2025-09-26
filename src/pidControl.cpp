#include "pidControl.h"
#include "config.h"

float previousError = 0.0;
float integral = 0.0;

const float intMin = -5000.0;
const float intMax = 5000.0;

float computePid(float setpoint, float currentSpeed)
{
    float error = setpoint - currentSpeed;

    if (setpoint == 0)
    {
        integral = 0.0;  
    }    else
    {
        integral += error;

        if (integral > intMax)
            integral = intMax;
        else if (integral < intMin)
            integral = intMin;
    }

    float derivative = error - previousError;
    
    previousError = error;

    float output = (kP * error) + (kI * integral) + (kD * derivative);

    return output;
}

void resetPid()
{
    previousError = 0.0;
    integral = 0.0;
}
