#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include <mcp_can.h>
#include "CANBusHandler.h"
#include "MotorRPMReader.h"
#include "pidControl.h"
#include "PS4ControllerHandler.h"

void driveWithPid(int forwardSetpoint, int sidewaysSetpoint, int turnSetpoint);

#endif 
