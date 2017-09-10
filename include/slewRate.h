#ifndef _SLEWRATE_H_
#define _SLEWRATE_H_

#include "globals.h"

extern int motorSlew[MOTOR_AMOUNT];//for each motor
extern int SlewAmount[MOTOR_AMOUNT];//for each motor

void MotorSlewRateTask(void * parameters);
//TaskHandle SlewRateMotorTask = taskCreate(MotorSlewRateTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);

#endif
