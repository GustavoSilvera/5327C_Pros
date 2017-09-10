#include "main.h"

void lift(int speed){
	//motorSlew[DannyLiftM] = speed;
	motorSet(DannyLiftML, speed);
	motorSet(DannyLiftMR, -speed);
}//function for controlling the danny lift (1 motor y cabled one motor has to be reversed)
void manualLiftControl(int min, int max, int sensorRead, int motor, int buttonUp, int buttonDown, bool reversed, bool slew){
    int dir = 1;
	if(reversed) dir = -1;
	if(sensorRead <= min && buttonUp == 1) 					{motorSet(motor, 0);return;}
	else if(sensorRead >= max && buttonDown == 1) 			{motorSet(motor, 0);return;}
	else if(buttonUp == 1 && sensorRead > min && slew)  	{motorSlew[motor] = dir * 127;}
	else if(buttonUp == 1 && sensorRead > min && !slew) 	{motorSet(motor, dir * 127);}
	else if (buttonDown == 1 && sensorRead < max && slew) 	{motorSlew[motor] = -dir * 127;}
	else if (buttonDown == 1 && sensorRead < max && !slew) 	{motorSet(motor, -dir * 127);}
	else if(!slew) 											{motorSet(motor, 0); return;}
	else /*yes slew*/										{motorSlew[motor] = 0; return;}
}
void MobileGoal(bool manual){//real noice rn
	if(manual) manualLiftControl(1450, 3250, analogRead(MoGoPot), MoGo, U5, D5, true, true);
	else ;//fancy pid
}//function for controlling the position of the mobile goal intake
void DannyLift(bool manual){
//basic lift control
	if(U6 == 1 || D6 == 1){
		if(U6 == 1 ) lift(127);
		if (D6 == 1) lift(-127);
	}
	else lift(0);
	//delay(10);
}//function for basic lift control via danny lift
void ChainBarCtrl(bool manual, struct PIDPar* CBar){
	if(manual) manualLiftControl(500, 2500, analogRead(CBarPot), ChainBar, U8, D8, false, false);
	else {
        CBar->isRunning = true;
		goalChainBar = 2000;//sets PID goal for chain bar
		if(U8 == 1 || D8 == 1) {manual = true;}
	}
}//function for basic lift control via chain bar
