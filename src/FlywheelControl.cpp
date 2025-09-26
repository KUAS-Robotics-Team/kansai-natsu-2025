#include "FlywheelControl.h"
#include "PS4ControllerHandler.h"
#include "CANBusHandler.h"
extern bool frontIsReversed;

void controlFlywheel()
{
    byte command = 0x00;

    if (PS4.isConnected())
    {
        if (frontIsReversed){
        if (PS4.Triangle())
        {
            command = 0x01;
            Serial.println ("0x01");
        }
        else if (PS4.Circle())
        {
            command = 0x02; 
            Serial.println ("0x02");

        }
        else if (PS4.Cross())
        {
            command = 0x03; 
            Serial.println ("0x03");

        }
        else if (PS4.Square())
        {
            command = 0x04; 
            Serial.println ("0x04");

        }

        else
        {
            command = 0x00; 
            Serial.println ("0x00");

        }
    }
    }

    byte data[8] = {command, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    sendCANMessage(0x301, data, 8); 

}
