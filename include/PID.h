#ifndef _PID_H_
#define _PID_H_

#include "globals.h"

struct PIDPar{
	char sensor;
	volatile int *goal;//changing asynchronously
	int motor1, motor2, thresh;
	float kP;
	bool slew, reversed, Mopposite;
	volatile bool isRunning;
};

void pidController(void * parameters);

#endif
