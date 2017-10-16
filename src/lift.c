#include "main.h"

void manualLiftControl(int min, int max, int sensorRead, int motor1, int motor2, int buttonUp, int buttonDown, int buttonUp2, int buttonDown2, bool reversed, bool slew, bool Mopposite, bool usingPID, int maxSpeed){
    int dir = 1;
	if(reversed) dir = -1;
    int moppo = 1;//switch ONE of the 2 motors (y cabled or opposite)
    if(Mopposite) moppo = -1;
    bool upButton = (buttonUp == 1 || buttonUp2 == 1);
    bool downButton = (buttonDown == 1 || buttonDown2 == 1);
    if(!upButton && !downButton) {motorSet(motor1, 0); motorSet(motor2, 0); return;}
    else if(sensorRead >= max && (upButton) ) 			{motorSet(motor1, 0); motorSet(motor2, 0); return;}///dont want to disturb the PID gods
	else if(sensorRead <= min && (downButton)) {motorSet(motor1, 0); motorSet(motor2, 0); return;}//dont want to disturb the PID gods
    //else if(sensorRead >= max && (buttonUp == 1 || buttonUp2 == 1) ) 	{motorSet(motor1, 0); motorSet(motor2, 0); return;}
	//else if(sensorRead <= min && (buttonDown == 1 || buttonDown2 == 1) ){motorSet(motor1, 0); motorSet(motor2, 0);return;}
	//slew//else if((buttonUp == 1 || buttonUp2 == 1) && sensorRead < max && slew)  	    {motorSlew[motor1] = dir * maxSpeed; motorSlew[motor2] = moppo * dir * maxSpeed;}
	else if((upButton) && sensorRead < max && !slew) 	    {motorSet(motor1, dir * maxSpeed); motorSet(motor2, moppo * dir * maxSpeed);}
	//slew//else if ((buttonDown == 1 || buttonDown2 == 1) && sensorRead > min && slew) 	{motorSlew[motor1] = -dir * maxSpeed; motorSlew[motor2] = moppo * -dir * maxSpeed;}
	else if ((downButton) && sensorRead > min && !slew)    {motorSet(motor1, dir * -maxSpeed); motorSet(motor2, moppo * dir * -maxSpeed);}
	else /*if(!slew)*/											                        {motorSet(motor1, 0); motorSet(motor2, 0); return;}
//	else /*yes slew*/										                        {motorSlew[motor1] = 0; motorSlew[motor2] = 0; return;}
}
void MoGoLift(int speed){
    motorSet(MoGoYCable, speed);
}
void MobileGoal(){//real noice rn
    manualLiftControl(MoGoMIN, MoGoMAX, analogRead(MoGoPot), MoGoYCable, 0, U5, D5, U52, D52, false, false, false, false, 127);

    /*if(U8 == 1 || D8 == 1 ){//}|| U5 == 1 || D5 == 1){
        MoGoAuton = false;
        int min = MoGoMIN + 400;
        int max = MoGoMAX - 400;
    //    if(analogRead(MoGoPot) <= min && (U8 == 1 || U5 == 1)) 			{motorSet(MoGo, 0);return;}///dont want to disturb the PID gods
    //	else if(analogRead(MoGoPot) >= max && (D8 == 1 || D5 == 1)) 	  {motorSet(MoGo, 0);return;}//dont want to disturb the PID gods
    //	else if( (U8 == 1 || U5 == 1) && analogRead(MoGoPot) > min )  	    {motorSet(MoGo, -127);}
    //	else if ( (D8 == 1 || D5 == 1) && analogRead(MoGoPot) < max) 	    {motorSet(MoGo, 127);}
    //	else               										        {motorSet(MoGo, 0);}

    }
	else{
        if(MoGoAuton){
            if(MoGoToggle) {//brings down
                if(analogRead(MoGoPot) > MoGoMIN) MoGoLift(-1 * abs(analogRead(MoGoPot) - MoGoMIN));
                else MoGoLift(0);
            }
            else{///brings up
                if(analogRead(MoGoPot) < MoGoMAX) MoGoLift(abs(analogRead(MoGoPot) - MoGoMAX));
                else MoGoLift(0);
            }
        }
    };*/
}//function for controlling the position of the mobile goal intake
void LiftLift(struct PIDPar* LiftPID, TaskHandle PIDTask){
//basic lift control
	if(U6 == 1 || D6 == 1 || U62 == 1 || D62 == 1){
        LiftPID->isRunning = false;
        manualLiftControl(LiftMIN, LiftMAX, analogRead(LiftPot), liftYCable, 0, U6, D6, U62, D62, false, false, false, false, 127);
	}
	else {
        if(!LiftPID->isRunning) goalLift = analogRead(LiftPot);
        LiftPID->isRunning = true;
    }
	//delay(10);
}//function for basic lift control via Lift lift
void FourBarCtrl(struct PIDPar* FourBar, TaskHandle PIDTask, TaskHandle SlewTask){
	if(U82 == 1 || D82 == 1 || U8 == 1 || D8 == 1) {
        taskSuspend(SlewTask);
        //taskSuspend(PIDTask);
        slewRunning = false;
        FourBar->isRunning = false;
        /*if(analogRead(FourBarPot) <= min && (U5 == 1 || U52 == 1)) 		 	{motorSet(FourBarYCable, 0); return;}///dont want to disturb the PID gods
        	else if(analogRead(FourBarPot) >= max && (D5 == 1 || D52 == 1)) 	    {motorSet(FourBarYCable, 0); return;}//dont want to disturb the PID gods
        	else if( (U5 == 1 || U52 == 1) && analogRead(FourBarPot) > min )  	    {motorSet(FourBarYCable, analogRead(FourBarPot) - FourBarMAX);}
            else if ( (D5 == 1 || D52 == 1) && analogRead(FourBarPot) < max) 	    {motorSet(FourBarYCable, analogRead(FourBarPot) - FourBarMIN);}
        	else               										            {motorSet(FourBarYCable, 0);}
            */
        manualLiftControl(FourBarMIN, FourBarMAX, analogRead(FourBarPot), FourBarYCable, 0, U8, D8, U82, D82, true, false, false, true, 80);
    }
	else {
        //taskResume(PIDTask);//turn on pid again
        if(!FourBar->isRunning){
            goalFourBar = analogRead(FourBarPot);//sets PID goal for chain bar
        }
        FourBar->isRunning = true;
    }
}//function for basic lift control via chain bar
