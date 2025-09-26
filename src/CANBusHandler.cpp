#include "CANBusHandler.h"
#include <config.h>


MCP_CAN CAN0(canCs);

bool initCAN() {
    if (CAN0.begin(MCP_ANY, CAN_1000KBPS, MCP_8MHZ) == CAN_OK) {
        Serial.println("MCP2515 Initialized Successfully!");
        CAN0.setMode(MCP_NORMAL);
        return true;
    } else {
        Serial.println("Error Initializing MCP2515...");
        return false;
    }
}


bool sendCANMessage(unsigned long id, byte data[], byte len) {
    byte sndStat = CAN0.sendMsgBuf(id, 0, len, data);
    if (sndStat == CAN_OK) {
        //Serial.println("Message Sent Successfully!");
        return true;
    } else {
       // Serial.println("Error Sending Message...");
        return false;
    }
}
