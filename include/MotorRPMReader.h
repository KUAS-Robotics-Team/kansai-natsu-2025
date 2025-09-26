#ifndef MOTOR_RPM_READER_H
#define MOTOR_RPM_READER_H

#include <Arduino.h>
#include "CANBusHandler.h"

struct MotorRPM {
    short M1;
    short M2;
    short M3;
    short M4;
};

extern MotorRPM motorRPMData;

void updateMotorRPM();

#endif
