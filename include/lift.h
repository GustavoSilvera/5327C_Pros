#ifndef _LIFT_H_
#define _LIFT_H_

#include "pid.h"

int goalChainBar;
int goalDanny;
int goalMoGo;

void lift(int speed);
void manualLiftControl(
    int min, int max, int sensorRead,
    int motor1, int motor2,
    int buttonUp, int buttonDown,
    bool reversed, bool slew, bool Mopposite, bool usingPID
);
void MobileGoal( struct PIDPar* MoGoPID);
void ChainBarCtrl(struct PIDPar* CBar);
void DannyLift( struct PIDPar* DannyPID);
#endif
