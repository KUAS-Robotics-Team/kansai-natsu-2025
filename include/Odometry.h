#pragma once
#include <Arduino.h>

// call once, from setup()
void beginOdometry();

// call each loop, from loop()
void pollOdometry();

// getters if you want to read pose from elsewhere:
double  posX();
double  posY();
double  yawRad();