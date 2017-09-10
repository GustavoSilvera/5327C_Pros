#include "main.h"

void pidController(void * parameters){
	struct PIDPar * tP = (struct PIDPar*) parameters;//creates pointer to tP (task parameters)
	int error;
	int minSpeed = 30;
	int dir = 1;
	if(tP->reversed) dir = -1;
	for(;;){//while true
        if(tP->isRunning){
        	error = (analogRead(tP->sensor) - *tP->goal);
        	if(abs(error) > minSpeed){
        		if(!tP->slew) motorSet(tP->motor, tP->kP * (dir * error));
        		else motorSlew[tP->motor] = tP->kP * (dir * error);//slewing PID
        	}
        	else if(abs(motorGet(tP->motor)) > 18){//lowest motor power until squeals
                if(!tP->slew) motorSet(tP->motor, dir * minSpeed);
        		else motorSlew[tP->motor] = (dir * minSpeed);//slewing PID
        	}
            else {//turn off motors
                if(!tP->slew) motorSet(tP->motor,0);
        		else motorSlew[tP->motor] = (0);//slewing PID
            }
        	delay(5);
        }
	}
	return;
}
