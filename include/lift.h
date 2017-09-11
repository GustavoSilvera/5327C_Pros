#ifndef _LIFT_H_
#define _LIFT_H_

#include "pid.h"

int goalChainBar;
int goalDanny;

void lift(int speed);
void manualLiftControl(
    int min, int max, int sensorRead,
    int motor1, int motor2,
    int buttonUp, int buttonDown,
    bool reversed, bool slew, bool Mopposite, bool usingPID);
void MobileGoal(bool manual);
void ChainBarCtrl(bool manual, struct PIDPar* CBar);
void DannyLift(bool manual, struct PIDPar* DannyPID);
#endif
