#ifndef _LIFT_H_
#define _LIFT_H_

#include "pid.h"

int goalChainBar;


void lift(int speed);
void manualLiftControl(int min, int max, int sensorRead, int motor, int buttonUp, int buttonDown, bool reversed, bool slew);
void MobileGoal(bool manual);
void ChainBarCtrl(bool manual, struct PIDPar* CBar);
void DannyLift(bool manual);
#endif
