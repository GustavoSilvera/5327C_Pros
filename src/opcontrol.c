#include "main.h"
#include <math.h>

struct robot current;

volatile bool toggle = false;

Gyro gyroscope;
Encoder encoder1;
Ultrasonic Usonic;

int currentTime = 0;

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
	goalFourBar = analogRead(FourBarPot);
	goalLift = analogRead(LiftPot);
}

void debug(){
	//printf("%d", encoderGet(encoder1));
	//printf("%d", digitalRead(button));//analogRead(FourBarPot));
	printf("%d", analogRead(FourBarPot));
	//printf("%d", ultrasonicGet(Usonic));
	//printf("%d", gyroGet(gyroscope));
	//printf("%f", velocity);
	printf("\n");
}//function for debugging sensor values and outputs to text terminal
//900
//1100
void grabStack(struct PIDPar* FourBar, struct PIDPar* LiftPID){//mini auton
	int initTime = millis();
	while(millis() - initTime < 5000){//time limit of 5 seconds
		FourBar->isRunning = true;
		LiftPID->isRunning = true;
		toggle = false;currentTime = millis();
		delay(100);
		goalLift = 950;
		goalFourBar = 150;
		if(abs(analogRead(LiftPot) - goalLift) > 200 && abs(analogRead(FourBarPot) - goalFourBar) > 150){
			delay(150);
			toggle = true;currentTime = millis();
			delay(300);
			break;
		}
	}
	initTime = millis();
	while(millis() - initTime < 3000){//time limit of 3 seconds
		FourBar->isRunning = true;
		LiftPID->isRunning = true;
		goalLift = 1200;
		goalFourBar = 3800;
		if(analogRead(FourBarPot) > 3500){
			delay(500);
			toggle = false;currentTime = millis();
			break;
		}
	}
	return;
}
void auton2(){//for ONLY the first cone
	goalFourBar = 1800;//have four bar go up sorta
	if(U7 == 1) return;
	goalLift = 1500;//have lift go up sorta
	if(U7 == 1) return;
	delay(800);//waits a sec
	if(U7 == 1) return;
	goalFourBar = 500;//brings 4 bar to pickup position
	if(U7 == 1) return;
	delay(400);
	if(U7 == 1) return;
	goalLift = 800;//goes down to picks up cone
	if(U7 == 1) return;
	delay(500);
	if(U7 == 1) return;
	goalFourBar = 1600;
	if(U7 == 1) return;
	delay(400);
	if(U7 == 1) return;
	goalLift = 1500;
	if(U7 == 1) return;
	delay(1000);
	if(U7 == 1) return;
	goalFourBar = FourBarMAX+100;
	if(U7 == 1) return;
	delay(500);
	if(U7 == 1) return;
	goalLift = 900;
	if(U7 == 1) return;
	delay(600);
	if(U7 == 1) return;
	delay(200);
	if(U7 == 1) return;
	goalFourBar = FourBarMAX - 300;
	if(U7 == 1) return;
	goalFourBar = FourBarMAX + 100;
	if(U7 == 1) return;
	delay(200);
	if(U7 == 1) return;
	goalFourBar = FourBarMAX - 300;
	if(U7 == 1) return;
	delay(200);
	if(U7 == 1) return;
	goalFourBar = FourBarMAX + 100;
	if(U7 == 1) return;
	delay(500);
	if(U7 == 1) return;
	goalFourBar = 1000;
	if(U7 == 1) return;
	return;
}
void auton(){//for every other cone
	if(U7 == 1) return;
	goalFourBar = 1800;//have four bar go up sorta
	if(U7 == 1) return;
	goalLift = 1500;//have lift go up sortaif(U7 == 1) return;
	if(U7 == 1) return;
	delay(800);//waits a sec
	if(U7 == 1) return;
	goalFourBar = 500;//brings 4 bar to pickup position
	if(U7 == 1) return;
	delay(400);
	if(U7 == 1) return;
	goalLift = 1500;//have lift go up sorta
	if(U7 == 1) return;
	goalLift = 800;//goes down to picks up cone
	if(U7 == 1) return;
	delay(500);
	if(U7 == 1) return;
	goalFourBar = 1600;
	if(U7 == 1) return;
	delay(400);
	if(U7 == 1) return;
	goalLift = 1500;
	if(U7 == 1) return;
	delay(1000);
	if(U7 == 1) return;
	goalFourBar = FourBarMAX+100;
	if(U7 == 1) return;
	delay(500);
	if(U7 == 1) return;
	goalLift = 900;
	if(U7 == 1) return;
	delay(500);
	if(U7 == 1) return;
	goalFourBar = 1000;
	if(U7 == 1) return;
	return;
}
void operatorControl(){//initializes everythin
	initializeOpControl();
	struct PIDPar * FourBarPID = (struct PIDPar *)malloc(sizeof(struct PIDPar));
	//allocates memory so is never destroyed and keeps running
	FourBarPID->sensor = FourBarPot;
	FourBarPID->goal = &goalFourBar;
	FourBarPID->motor1 = FourBarYCable;
	FourBarPID->motor2 = 0;//no second motor used
	FourBarPID->thresh = 30;
	FourBarPID->kP = 0.15;//pretty efficient lift tho0.075
	FourBarPID->slew = false;
	FourBarPID->reversed = false;
	FourBarPID->isRunning = false;
	FourBarPID->Mopposite = false;

	struct PIDPar * LiftPID = (struct PIDPar *)malloc(sizeof(struct PIDPar));//allocates in memory
	LiftPID->sensor = LiftPot;
	LiftPID->goal = &goalLift;
	LiftPID->motor1 = liftYCable;
	LiftPID->motor2 = 0;
	LiftPID->thresh = 10;
	LiftPID->kP = 0.25;//lol lift is not vry efficient0.08
	LiftPID->slew = false;
	LiftPID->reversed = true;
	LiftPID->isRunning = false;
	LiftPID->Mopposite = false;//if motors controlled by the lift should go different directions

	TaskHandle FourBarPIDTask = taskCreate(pidController, TASK_DEFAULT_STACK_SIZE, FourBarPID, TASK_PRIORITY_DEFAULT);
	TaskHandle LiftPIDTask = taskCreate(pidController, TASK_DEFAULT_STACK_SIZE, LiftPID, TASK_PRIORITY_DEFAULT);
	TaskHandle SlewRateMotorTask = taskCreate(MotorSlewRateTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	TaskHandle Speedometer = taskCreate(MeasureSpeed, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);

	taskResume(Speedometer);
	taskResume(SlewRateMotorTask);
	taskResume(FourBarPIDTask);
	taskResume(LiftPIDTask);
	while(true){
		debug();
		drive(false);
		FourBarCtrl(FourBarPID, FourBarPIDTask, SlewRateMotorTask);
		LiftLift(LiftPID, LiftPIDTask);
		MobileGoal();

		if(D7 == 1 || D72 == 1) {grabStack(FourBarPID, LiftPID); }
		//if(R8 == 1 || R82 == 1) {MoGoToggle = !MoGoToggle; MoGoAuton = true; delay(250);}
		if(R7 == 1 || R72 == 1) auton();//max can precicely drive is 50
		if(L7 == 1 || L72 == 1) auton2();//max can precicely drive is 50
		if(L7 == 1 || L72 == 1) goalFourBar = 500;
	}
}//function for operator control
