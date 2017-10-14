#ifndef _LIFT_H_
#define _LIFT_H_

#include "pid.h"

int goalFourBar;
int goalLift;
int goalMoGo;

void lift(int speed);
void manualLiftControl(
    int min, int max, int sensorRead,
    int motor1, int motor2,
    int buttonUp, int buttonDown, int buttonUp2, int buttonDown2,
    bool reversed, bool slew, bool Mopposite, bool usingPID,
    int maxSpeed
);
void MoGoLift(int speed);
void MobileGoal();
void LiftLift( struct PIDPar* LiftPID, TaskHandle PIDTask);
void FourBarCtrl(struct PIDPar* CBar, TaskHandle PIDTask, TaskHandle SlewTask);
#endif
