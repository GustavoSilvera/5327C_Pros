#include "main.h"

void manualLiftControl(int min, int max, int sensorRead, int motor1, int motor2, int buttonUp, int buttonDown, bool reversed, bool slew, bool Mopposite, bool usingPID){
    int dir = 1;
	if(reversed) dir = -1;
    int moppo = 1;//switch ONE of the 2 motors (y cabled or opposite)
    if(Mopposite) moppo = -1;
    if(sensorRead <= min && buttonUp == 1 && usingPID) 			{return;}///dont want to disturb the PID gods
	else if(sensorRead >= max && buttonDown == 1 && usingPID) 	{return;}//dont want to disturb the PID gods
    else if(sensorRead <= min && buttonUp == 1 && !usingPID) 	{motorSet(motor1, 0); motorSet(motor2, 0); return;}
	else if(sensorRead >= max && buttonDown == 1 && !usingPID) 	{motorSet(motor1, 0); motorSet(motor2, 0);return;}
	else if(buttonUp == 1 && sensorRead > min && slew)  	    {motorSlew[motor1] = dir * 127; motorSlew[motor2] = moppo * dir * 127;}
	else if(buttonUp == 1 && sensorRead > min && !slew) 	    {motorSet(motor1, dir * 127); motorSet(motor2, moppo * dir * 127);}
	else if (buttonDown == 1 && sensorRead < max && slew) 	    {motorSlew[motor1] = -dir * 127; motorSlew[motor2] = moppo * -dir * 127;}
	else if (buttonDown == 1 && sensorRead < max && !slew) 	    {motorSet(motor1, -dir * 127); motorSet(motor2, moppo * -dir * 127);}
	else if(!slew) 											    {motorSet(motor1, 0); motorSet(motor2, 0); return;}
	else /*yes slew*/										    {motorSlew[motor1] = 0; motorSlew[motor2] = 0; return;}
}
void MobileGoal(bool manual){//real noice rn
	if(manual) manualLiftControl(1450, 3250, analogRead(MoGoPot), MoGo, 0, U5, D5, true, false, false, false);
	else ;//fancy pid
}//function for controlling the position of the mobile goal intake
void DannyLift(bool manual, struct PIDPar* DannyPID){
//basic lift control
	if(U6 == 1 || D6 == 1){
        DannyPID->isRunning = false;
        manualLiftControl(800, 2800, analogRead(DannyPot), DannyLiftMR, DannyLiftML, U6, D6, true, false, true, true);
	}
	else {
        if(!DannyPID->isRunning){
            goalDanny = analogRead(DannyPot);
        }
        DannyPID->isRunning = true;
    }
	//delay(10);
}//function for basic lift control via danny lift
void ChainBarCtrl(bool manual, struct PIDPar* CBar){
	if(U8 == 1 || D8 == 1) {
        CBar->isRunning = false;
        manualLiftControl(500, 2500, analogRead(CBarPot), ChainBar, 0, U8, D8, false, false, false, true);
    }
	else {
        if(!CBar->isRunning){
		    goalChainBar = analogRead(CBarPot);//sets PID goal for chain bar
        }
        CBar->isRunning = true;
    }
}//function for basic lift control via chain bar
