
#include "odometry.h"
#include <Arduino.h>
#include <cmath>

static constexpr int    PULSES_PER_REV   = 4096;
static constexpr double WHEEL_DIA_M      = 0.05;
static constexpr double DIST_PER_COUNT   = (WHEEL_DIA_M * M_PI) / PULSES_PER_REV;
static constexpr double POD_RAD_M        = 0.10;
static constexpr double TWO_R            = 2.0 * POD_RAD_M;

static constexpr uint8_t POD1_A = 17;
static constexpr uint8_t POD1_B = 16;
static constexpr uint8_t POD2_A = 12;
static constexpr uint8_t POD2_B = 13;

static volatile int pod1Sign = +1;
static volatile int pod2Sign = +1;

void setEncoderPolarity(int p1, int p2)
{
    pod1Sign = (p1 >= 0) ? +1 : -1;
    pod2Sign = (p2 >= 0) ? +1 : -1;
}

static volatile long counter1 = 0;
static volatile long counter2 = 0;
static volatile bool lastA1   = false;
static volatile bool lastA2   = false;

static void IRAM_ATTR isrPod1()
{
    bool a = digitalRead(POD1_A);
    bool b = digitalRead(POD1_B);
    if (a != lastA1)
    {
        counter1 += (a == b) ? -1 : +1;
        lastA1 = a;
    }
}

static void IRAM_ATTR isrPod2()
{
    bool a = digitalRead(POD2_A);
    bool b = digitalRead(POD2_B);
    if (a != lastA2)
    {
        counter2 += (a == b) ? -1 : +1;
        lastA2 = a;
    }
}


static double xPos = 0.0;
static double yPos = 0.0;
static double heading = 0.0;

void beginOdometry()
{
    pinMode(POD1_A, INPUT_PULLUP);
    pinMode(POD1_B, INPUT_PULLUP);
    pinMode(POD2_A, INPUT_PULLUP);
    pinMode(POD2_B, INPUT_PULLUP);

    lastA1 = digitalRead(POD1_A);
    lastA2 = digitalRead(POD2_A);

    counter1 = 0;
    counter2 = 0;

    attachInterrupt(POD1_A, isrPod1, CHANGE);
    attachInterrupt(POD2_A, isrPod2, CHANGE);

    xPos = yPos = heading = 0.0;
}

void pollOdometry()
{
    noInterrupts();
    long c1 = counter1;
    long c2 = counter2;
    interrupts();

    static long prev1 = 0;
    static long prev2 = 0;

    long dc1 = c1 - prev1;
    long dc2 = c2 - prev2;
    prev1 = c1;
    prev2 = c2;

    if (dc1 == 0 && dc2 == 0) return;

    const double s1 = dc1 * DIST_PER_COUNT * pod1Sign;
    const double s2 = dc2 * DIST_PER_COUNT * pod2Sign;

    const double dTheta = (s2 - s1) / TWO_R;
    const double dXb    = s1 + POD_RAD_M * dTheta;
    const double dYb    = s2 - POD_RAD_M * dTheta;

    heading += dTheta;
    if (heading >  M_PI) heading -= 2.0 * M_PI;
    if (heading <= -M_PI) heading += 2.0 * M_PI;

    const double ch = cos(heading);
    const double sh = sin(heading);
    xPos += dXb * ch - dYb * sh;
    yPos += dXb * sh + dYb * ch;
}


double posX()   { return xPos;     }
double posY()   { return yPos;     }
double yawRad() { return heading;  }

void getPose(double &x, double &y, double &th)
{
    x  = xPos;
    y  = yPos;
    th = heading;
}

void resetPose(double x, double y, double th)
{
    xPos   = x;
    yPos   = y;
    heading = th;
}
