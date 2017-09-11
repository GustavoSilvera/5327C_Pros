#ifndef _BASE_H_
#define _BASE_H_
#include "globals.h"

float TruSpeed(float value);
void drive();
void fwds(int speed);
void rot(int speed);
void rotFor(int goal);
void driveFor(float goal);
void updateNav();

#endif
