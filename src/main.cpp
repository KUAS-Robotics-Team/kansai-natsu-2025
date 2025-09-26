#include <Arduino.h>
#include "config.h"
#include "PS4ControllerHandler.h"
#include "BluetoothUtils.h"
#include "MotorControl.h"
#include "CANBusHandler.h"
#include "DisplayHandler.h"
#include "MotorRPMReader.h"
#include <IntakeControl.h>
#include "FlywheelControl.h"
#include "odometry.h"
#include "LiftControl.h"
#include "ClawControl.h"

bool frontIsReversed = false;
bool isPrecisionMode = false;

void setup()
{
  Serial.begin(115200);
  beginOdometry();
  
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);
  delay(50);
  digitalWrite(buzzerPin, LOW);


  if (!initDisplay())
  {
    Serial.println("Display Initialization Failed!");
  }
  else
  {
    showMessage("Pika");
  }

  if (!initCAN())
  {
    Serial.println("CAN Initialization Failed!");
  }


  PS4.begin();
  removePairedDevices();
  Serial.print("This device MAC is: ");
  printDeviceAddress();
  Serial.println("Ready.");
  delay(5000);
}

void loop()
{

  checkPS4(maxSpeed);
  driveWithPid(forwardSetpoint, sidewaysSetpoint, turnSetpoint);


  // pollOdometry();


  controlIntake();
  controlFlywheel();
  controlLift();
  controlClaw();

  // static unsigned long lastDisplay = 0;
  // unsigned long now = millis();
  // if (now - lastDisplay >= 100)
  // {
  //   lastDisplay = now;

  //   float x = 100 * posX();
  //   float y = 100 * posY();
  //   float h = yawRad();

  //   char buf[40];
  //   snprintf(buf, sizeof(buf), "X:%.2f Y:%.2f H:%.2f", x, y, h);
  //   showMessage(buf);
  // }
}
