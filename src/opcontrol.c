#include "main.h"
#include <math.h>

struct robot current;

volatile bool toggle = false;

Gyro gyroscope;
Encoder encoder1;
Ultrasonic Usonic;

void initializeOpControl(){
	//PID.requestedValue = 200;
	//PID.isRunning = false;
	delay(100);
	slewRunning = false;
	encoderReset(encoder1);
	gyroShutdown(gyroscope);//resets gyro sensor, rly sketchy
	gyroscope = gyroInit(1,0);
	current.Xpos = 0.0;
	current.Ypos = 0.0;
	current.deg = 0.0;
	velocity = 0.0;
	goalChainBar = analogRead(CBarPot);
	goalDanny = analogRead(DannyPot);
	goalMoGo = analogRead(MoGoPot);
	MoGoToggle = true;//brings mogo to the front
}

void debug(){
	//printf("%d", encoderGet(encoder1));
	printf("%d", digitalRead(button));//analogRead(CBarPot));
	//printf("%d", analogRead(CBarPot));
	//printf("%d", ultrasonicGet(Usonic));
	//printf("%d", gyroGet(gyroscope));
	//printf("%f", velocity);
	printf("\n");
}//function for debugging sensor values and outputs to text terminal


void grabStack(struct PIDPar* CBar, struct PIDPar* DannyPID){//mini auton
	int initTime = millis();
	while(millis() - initTime < 5000){//time limit of 5 seconds
		CBar->isRunning = true;
		DannyPID->isRunning = true;
		goalDanny = 950;
		goalChainBar = 200;
		toggle = false;
		if(abs(analogRead(DannyPot) - goalDanny) > 200 && abs(analogRead(CBarPot) - goalChainBar) > 150){
			delay(70);
			toggle = true;
			delay(300);
			break;
		}
	}
	initTime = millis();
	while(millis() - initTime < 3000){//time limit of 3 seconds
		CBar->isRunning = true;
		DannyPID->isRunning = true;
		goalDanny = 1200;
		goalChainBar = 3800;
		if(analogRead(CBarPot) > 3500){
			delay(300);
			toggle = false;
			break;
		}
	}
	return;
}
void claw(){
	int currentTime = 0;
	int outTime = 0.5;//seconds of bringing claw outwards
	for(;;){
		if(toggle)	{
			if(digitalRead(button) == HIGH)	motorSet(Claw, 127);//if pressed
			currentTime = millis();
		}
		else {
			if(millis() - currentTime < outTime * 1000) motorSet(Claw, -127);
			else motorSet(Claw, 0);
		}

		delay(40);
	}
}
void operatorControl(){//initializes everythin
	initializeOpControl();

	struct PIDPar * CBarPID = (struct PIDPar *)malloc(sizeof(struct PIDPar));
	//allocates memory so is never destroyed and keeps running
	CBarPID->sensor = CBarPot;
	CBarPID->goal = &goalChainBar;
	CBarPID->motor1 = ChainBar;
	CBarPID->motor2 = 0;//no second motor used
	CBarPID->thresh = 30;
	CBarPID->kP = 0.075;//pretty efficient lift tho
	CBarPID->slew = false;
	CBarPID->reversed = false;
	CBarPID->isRunning = false;
	CBarPID->Mopposite = false;

	struct PIDPar * DannyPID = (struct PIDPar *)malloc(sizeof(struct PIDPar));//allocates in memory
	DannyPID->sensor = DannyPot;
	DannyPID->goal = &goalDanny;
	DannyPID->motor1 = DannyLiftMR;
	DannyPID->motor2 = DannyLiftML;
	DannyPID->thresh = 20;
	DannyPID->kP = 0.2;//lol lift is not vry efficient
	DannyPID->slew = false;
	DannyPID->reversed = false;
	DannyPID->isRunning = false;
	DannyPID->Mopposite = true;//if motors controlled by the lift should go different directions

	struct PIDPar * MoGoPID = (struct PIDPar *)malloc(sizeof(struct PIDPar));//allocates in memory
	MoGoPID->sensor = MoGoPot;
	MoGoPID->goal = &goalMoGo;
	MoGoPID->motor1 = MoGo;
	MoGoPID->motor2 = 0;//no second motor
	MoGoPID->thresh = 40;
	MoGoPID->kP = 0.4;//lol lift is not vry efficient
	MoGoPID->slew = false;
	MoGoPID->reversed = true;
	MoGoPID->isRunning = true;
	MoGoPID->Mopposite = false;//if motors controlled by the lift should go different directions

	TaskHandle CBarPIDTask = taskCreate(pidController, TASK_DEFAULT_STACK_SIZE, CBarPID, TASK_PRIORITY_DEFAULT);
	TaskHandle MoGoPIDTask = taskCreate(pidController, TASK_DEFAULT_STACK_SIZE, MoGoPID, TASK_PRIORITY_DEFAULT);
	TaskHandle DannyPIDTask = taskCreate(pidController, TASK_DEFAULT_STACK_SIZE, DannyPID, TASK_PRIORITY_DEFAULT);
	TaskHandle SlewRateMotorTask = taskCreate(MotorSlewRateTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	TaskHandle Speedometer = taskCreate(MeasureSpeed, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	TaskHandle clawControl = taskCreate(claw, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);

	taskResume(Speedometer);
	taskResume(SlewRateMotorTask);
	taskResume(CBarPIDTask);
	taskResume(DannyPIDTask);
	taskResume(MoGoPIDTask);
	taskResume(clawControl);
	while(true){
		debug();
		drive(false);
		ChainBarCtrl(CBarPID, CBarPIDTask, SlewRateMotorTask);
		DannyLift(DannyPID, DannyPIDTask);
		MobileGoal(MoGoPID, MoGoPIDTask);
		if(D7 == 1 || D72 == 1){ grabStack(CBarPID, DannyPID); }
		if(L8 == 1 || L82 == 1 ){ toggle = !toggle; delay(250);}//claw control
		if(U8 == 1 || D8 == 1) MoGoToggle = !MoGoToggle;
		//intake();
		if(R7 == 1 || R72 == 1) driveFor(20);//max can precicely drive is 50
		if(L7 == 1 || L72 == 1) rotFor(-90);
		delay(5);
	}
}//function for operator control
