#include "MotorRPMReader.h"
#include "config.h"

MotorRPM motorRPMData = {0, 0, 0, 0};
void updateMotorRPM() {
    unsigned long rxId;
    unsigned char len = 0;
    unsigned char rxBuf[8];

    if (!digitalRead(canInt)) { 
        CAN0.readMsgBuf(&rxId, &len, rxBuf);

        short rpm = (rxBuf[2] << 8) | rxBuf[3];

        switch (rxId) {
            case 0x201:
                motorRPMData.M1 = rpm;
                break;
            case 0x202:
                motorRPMData.M2 = rpm;
                break;
            case 0x203:
                motorRPMData.M3 = rpm;
                break;
            case 0x204:
                motorRPMData.M4 = rpm;
                break;
            default:
                return;
        }   
    }
}
