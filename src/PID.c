#include "main.h"

void pidController(void * parameters){
	struct PIDPar * tP = (struct PIDPar*) parameters;//creates pointer to tP (task parameters)
	int error;
	int minSpeed = 30;
	int dir = 1;
	if(tP->reversed) dir = -1;
	int moppo;
	if(tP->Mopposite) moppo = -1;
	for(;;){//while true
        if(false){//tP->isRunning){
        	error = (analogRead(tP->sensor) - *tP->goal);
        	if(abs(error) > minSpeed){
				float power = tP->kP * (dir * error);
        		if(!tP->slew) {
					motorSet(tP->motor1, power);
					motorSet(tP->motor2, moppo * power);
				}
        		else{
					 motorSlew[tP->motor1] = power;//slewing PID
					 motorSlew[tP->motor2] = moppo * power;//slewing PID
				 }
        	}
        	else if(abs(motorGet(tP->motor1)) > 20){//lowest motor power until squeals
                float power = dir * minSpeed;
				if(!tP->slew) {
					motorSet(tP->motor1, power);
					motorSet(tP->motor2, moppo * power);
				}
        		else{
					 motorSlew[tP->motor1] = power;//slewing PID
					 motorSlew[tP->motor2] = moppo * power;//slewing PID
				 }
        	}
            else {//turn off motors
				if(!tP->slew) {
					motorSet(tP->motor1, 0);
					motorSet(tP->motor2, 0);
				}
        		else{
					 motorSlew[tP->motor1] = 0;//slewing PID
					 motorSlew[tP->motor2] = 0;//slewing PID
				 }
            }
        	delay(2);
        }
	}
}
