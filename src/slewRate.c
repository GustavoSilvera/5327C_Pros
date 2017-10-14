#include "main.h"
#include "globals.h"

int motorSlew[MOTOR_AMOUNT];//for each motor
int SlewAmount[MOTOR_AMOUNT];//for each motor

void MotorSlewRateTask( void * parameters){//slew rate task
	printf("slewRate");
	int motorI;//which motor are we talking about? INDEX
	//for(motorI = 0; motorI <= MOTOR_AMOUNT; motorI++){//resets for each motor
	//	motorSlew[motorI] = 0;//for each motor, the requested speed is 0
	//	SlewAmount[motorI] = 20;//allow the initial slew change to be 15 (anything else and the motor basically wont move)
	//}//USED FOR INITIALIZING EVERY MOTOR_AMOUNT
	SlewAmount[useless] = 10;//useless motre anyways.
	SlewAmount[RightBase] = 50;//lots o' speed
	SlewAmount[RightBaseYCable] = 50;//lots o' speed
	SlewAmount[liftYCable] = 40;//want more speed
	SlewAmount[FourBarYCable] = 40;//want more speed
	SlewAmount[MoGoYCable] = 10;//higher torque
	while(true){// run loop for every motor
		if(slewRunning){
			for( motorI = 0; motorI <= MOTOR_AMOUNT; motorI++){
				//motorCurrent = motor[ motorIndex ];
				if( abs(motorGet(motorI) - motorSlew[motorI]) > 10){//current motor value not equal to goal
					if( motorGet(motorI) < motorSlew[motorI] && motorGet(motorI) < 127){//if less than goal || less than max
						motorSet(motorI, motorGet(motorI) + SlewAmount[motorI]);//decrease by specific amount
						if( motorGet(motorI) >= motorSlew[motorI]){//if equal to or surpassed goal
							motorSet(motorI, motorSlew[motorI]);//sets change to goal
						}
					}
					if( motorGet(motorI) > motorSlew[motorI] && motorGet(motorI) > -127){//if currently more than requested
						motorSet(motorI, motorGet(motorI) - SlewAmount[motorI]);//decrease by specific amount
						if(motorGet(motorI) <= motorSlew[motorI]){//once reaches or passes goal
							motorSet(motorI, motorSlew[motorI]);//sets change to goal
						}
					}
					motorSet(motorI, motorGet(motorI));
				}
				delay(5);//delay 25ms
			}
		}
	}
}//task for "slew"-ing the motres by adding to their power via loops
