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
	encoderReset(encoder1);
	gyroShutdown(gyroscope);//resets gyro sensor, rly sketchy
	gyroscope = gyroInit(1,0);
	current.Xpos = 0.0;
	current.Ypos = 0.0;
	current.deg = 0.0;
	velocity = 0.0;
	goalChainBar = analogRead(CBarPot);
}

void debug(){
	//printf("%d", encoderGet(encoder1));
	//printf("%d", analogRead(CBarPot));
	//printf("%d", ultrasonicGet(Usonic));
	//printf("%d", gyroGet(gyroscope));
	printf("%f", velocity);
	printf("\n");
}//function for debugging sensor values and outputs to text terminal

void ClawIntake(){//real noice rn
	if(U7 == 1 || D7 == 1){
		if(U7 == 1 ) motorSet(Claw, 127);
		if (D7 == 1) motorSet(Claw, -127);
	}
	else motorSet(Claw, 0);
}//function for controlling the position of the mobile goal intake
void intake(){//for pneumatics
    if(U8 == 1) toggle = !toggle;
    if(toggle) digitalWrite(3, HIGH);
    else digitalWrite(3, LOW);
    delay(200);
}//function for current intake (for pneumatic claw)
void operatorControl(){//initializes everythin
	initializeOpControl();

	struct PIDPar * CBarPID = (struct PIDPar *)malloc(sizeof(struct PIDPar));
	//allocates memory so is never destroyed and keeps running
	CBarPID->sensor = CBarPot;
	CBarPID->goal = &goalChainBar;
	CBarPID->motor = ChainBar;
	CBarPID->thresh = 30;
	CBarPID->kP = 0.075;
	CBarPID->slew = false;
	CBarPID->reversed = false;

	TaskHandle PIDTask = taskCreate(pidController, TASK_DEFAULT_STACK_SIZE, CBarPID, TASK_PRIORITY_DEFAULT);

	TaskHandle SlewRateMotorTask = taskCreate(MotorSlewRateTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	TaskHandle Speedometer = taskCreate(MeasureSpeed, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);

	taskResume(Speedometer);
	taskResume(SlewRateMotorTask);
	taskResume(PIDTask);
	while(true){
		debug();
		drive();
		ChainBarCtrl(false, CBarPID);
		//intake();
	//	if(R8 == 1) driveFor(35);
		//if(R7 == 1) driveFor(40);//max can precicely drive is 50
		ClawIntake();
		MobileGoal(true);
		DannyLift(false);
		delay(3);
	}
}//function for operator control
