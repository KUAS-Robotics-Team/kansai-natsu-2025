#include <SPI.h>
#include <Servo.h>
#include "mcp_can.h"

const int SPARK_MAX_PIN = 3;

const int SPI_CS_PIN = 10;

const long unsigned int TARGET_CAN_ID = 0x301;

const int MOTOR_SPEED_KICK = 2000;
const int MOTOR_SPEED_LOWLOW = 1750;
const int MOTOR_SPEED_LOWHIGH = 1770;
const int MOTOR_SPEED_HIGHLOW = 1800;
const int MOTOR_SPEED_HIGHHIGH = 1823;
const int MOTOR_SPEED_NEUTRAL = 1500;
const int KICK_DURATION_MS = 75;

const unsigned long CAN_TIMEOUT = 200;

Servo sparkMax;

MCP_CAN CAN(SPI_CS_PIN);

unsigned long lastMessageTime = 0;

enum MotorState { STOPPED, KICKING, RUNNING };
MotorState motorState = STOPPED;
unsigned long kickStartTime = 0;
int currentRunSpeed = MOTOR_SPEED_NEUTRAL;


void setup() {
  Serial.begin(115200);

  sparkMax.attach(SPARK_MAX_PIN);
  sparkMax.writeMicroseconds(MOTOR_SPEED_NEUTRAL);

  Serial.println("Initializing MCP2515 CAN Controller...");

  while (CAN.begin(MCP_ANY, CAN_1000KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("Error Initializing MCP2515. Retrying...");
    delay(200);
  }
  Serial.println("MCP2515 Initialized Successfully!");

  CAN.setMode(MCP_NORMAL);

  pinMode(2, INPUT);

  Serial.println("Setup complete. Waiting for CAN messages...");
}

void handleCANMessage() {
  long unsigned int rxId;
  byte len = 0;
  byte rxBuf[8];

  CAN.readMsgBuf(&rxId, &len, rxBuf);

  if (rxId == TARGET_CAN_ID) {
    lastMessageTime = millis();

    byte command = (len > 0) ? rxBuf[0] : 0xFF;

    bool startMotor = false;

    if (command == 0x01) {
      currentRunSpeed = MOTOR_SPEED_LOWLOW;
      startMotor = true;
    } else if (command == 0x02) {
      currentRunSpeed = MOTOR_SPEED_LOWHIGH;
      startMotor = true;
    } else if (command == 0x03) {
      currentRunSpeed = MOTOR_SPEED_HIGHLOW;
      startMotor = true;
    } else if (command == 0x04) {
      currentRunSpeed = MOTOR_SPEED_HIGHHIGH;
      startMotor = true;
    } else if (command == 0x00) {
      motorState = STOPPED;
      sparkMax.writeMicroseconds(MOTOR_SPEED_NEUTRAL);
      startMotor = false;
    }

    if (startMotor && motorState == STOPPED) {
      motorState = KICKING;
      kickStartTime = millis();
      sparkMax.writeMicroseconds(MOTOR_SPEED_KICK);
    }
  }
}

void updateMotorState() {
  if (motorState == KICKING && (millis() - kickStartTime > KICK_DURATION_MS)) {
    motorState = RUNNING;
    sparkMax.writeMicroseconds(currentRunSpeed);
  }

  if (millis() - lastMessageTime > CAN_TIMEOUT) {
    if (motorState != STOPPED) {
      motorState = STOPPED;
      sparkMax.writeMicroseconds(MOTOR_SPEED_NEUTRAL);
    }
  }
}

void loop() {
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    handleCANMessage();
  }

  updateMotorState();
}
