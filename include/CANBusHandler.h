#ifndef CAN_BUS_HANDLER_H
#define CAN_BUS_HANDLER_H

#include <mcp_can.h>
#include <SPI.h>

extern MCP_CAN CAN0;

bool initCAN();

bool sendCANMessage(unsigned long id, byte data[], byte len);


#endif 
