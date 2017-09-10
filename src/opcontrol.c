#include "main.h"
#include <math.h>
#include "slewRate.c"

struct robot current;
float circum = 4*PI;
float velocity;
//volatile struct maintainPosition PID;

//const in c means only READ ONLY
// Array for requested speed of motors
volatile bool toggle = false;

Gyro gyroscope;
Encoder encoder1;
Ultrasonic Usonic;
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
	SlewAmount[RightBaseM] = 50;//lots o' speed
	SlewAmount[LeftBaseM] = 50;//lots o' speed
	SlewAmount[DannyLiftML] = 20;//want more speed
	SlewAmount[DannyLiftMR] = 20;//want more speed
	SlewAmount[MoGo] = 10;//higher torque
	SlewAmount[ChainBar] = 20;
	SlewAmount[Claw] = 10;
	while(true){// run loop for every motor
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
			delay(25);//delay 25ms
		}
	}
}//task for "slew"-ing the motres by adding to their power via loops
void MeasureSpeed(void * parameters){
	/*MEASURING IN IN/SEC*/
	int encoderPast = 0;
	float delayAmount = 50;
	while(true){
		velocity = ( (encoderGet(encoder1) - encoderPast) / circum) / (delayAmount/1000);//1000 ms in 1s
		encoderPast = encoderGet(encoder1);
		delay(delayAmount);
	}
}//task for measuring velocity of the base, in IN/Sec
void initializeOpControl(){
	//PID.requestedValue = 200;
	//PID.isRunning = false;
	encoderReset(encoder1);
	gyroShutdown(gyroscope);//resets gyro sensor, rly sketchy
	gyroscope = gyroInit(1,0);
	current.Xpos = 0.0;
	current.Ypos = 0.0;
	current.deg = 0.0;
	velocity = 0;
}
float avg(float val1, float val2){
	return (val1 + val2 ) * 0.5;
}//func for avg
float getSign(float check){
	if (check < 0) return -1;
	else if (check > 0) return 1;
	return 0;
}//func for getting sign of a number
void lift(int speed){
	//motorSlew[DannyLiftM] = speed;
	motorSet(DannyLiftML, speed);
	motorSet(DannyLiftMR, -speed);
}//function for controlling the danny lift (1 motor y cabled one motor has to be reversed)
void pidController(void * parameters){
/*	float kP = 0.25;//remove later
	float kI = 15;//remove later
	float kD = 0.0;//remove later
    // If we are using an encoder then clear it
	//encoderReset(encoder1);
    PID.lastError = 0;
    PID.integral = 0;
	PID.error = 0;
	PID.derivative = 0;
	PID.currentPos = PIDSensorType;
	PID.threshold = 20;//sets error threshold
    while(true){
		if(PID.isRunning){
			//initializePID
			//start main loop
			PID.currentPos = PIDSensorType;// * sensorScale;idk if i need this, probs not.
			if(abs(PID.currentPos - PID.requestedValue) > PID.threshold)//if not within threshold
				PID.error = PID.currentPos - PID.requestedValue;//calculate directly proportional error
			else PID.error = 0;//else no error, its good enough
			if( kI != 0 ){
                if( abs(PID.error) < PID_INTEGRAL_LIMIT ) PID.integral += PID.error;//used for averaging the integral amount, later in motor power divided by 25
                else PID.integral = 0;
            }
            else
            	PID.integral = 0;
        	// calculate the derivative
        	PID.derivative = PID.error - PID.lastError;
        	PID.lastError  = PID.error;
        	// calculate drive
				int power = (kP * PID.error);// + (PID.integral / kI) + (kD * PID.derivative) ;
			if(abs(power) > 15){
				//NO SLEW RATE (DOSENT UPDATE AS FAST)
				lift(power);
			}
    		// Run at 50Hz
		}
		else{
			PID.lastError = 0;
			PID.integral = 0;
			PID.error = 0;
			PID.derivative = 0;
		}
    }*/
}//task for a PID lift controller, not implemented well YET, ignore for now
void PID(char sensor, int goal, int motor, int thresh, float kP, bool slew, bool reversed){
	int error;
	int minSpeed = 30;
	int dir = 1;
	if(reversed) dir = -1;
	do {
		error = (analogRead(sensor) - goal);
		if(abs(error) > minSpeed){
			if(!slew) motorSet(motor, kP * (dir * error));
			else motorSlew[motor] = kP * (dir * error);//slewing PID
		}
		else {
			if(!slew) motorSet(motor, dir * minSpeed);
			else motorSlew[motor] = (dir * minSpeed);//slewing PID
		}
		delay(5);
	} while(abs(error) > thresh);
	return;
}
float TruSpeed(float value){
	//for all other polynomials; visit: goo.gl/mhvbx4
	return(getSign(value)*(value*value) /(127));
}//function for calculating the truSpeed function based off a quadratic polynomial
void drive(){
	//motorSlew[RightBaseM] = TruSpeed(joystickGetAnalog(1,2));//y axis for baseRight joystick
	//motorSlew[LeftBaseM] = TruSpeed(joystickGetAnalog(1,3));//y acis for left  joystick
	//noSLEW
	motorSet(RightBaseM, TruSpeed(joystickGetAnalog(1,2)));
	motorSet(LeftBaseM, TruSpeed(joystickGetAnalog(1,3)));
}//function for driving the robot
void fwds(int speed){
	motorSet(RightBaseM, speed);
	motorSet(LeftBaseM, speed);
}
void driveFor(float goal){
	encoderReset(encoder1);
	int thresh = 10;//10 ticks
	int StartTime = millis();
	while ( abs(goal*circum - encoderGet(encoder1)) > thresh){
		if(millis() - StartTime < abs(goal) * 1000){
			fwds((goal*circum - encoderGet(encoder1) - velocity) *(10/(goal)));//SO GOOD
		}
		else return;//do something fancy for checking stalls
	}
	return;
}
//ignore// function for driving the robot a little bit
void updateNav(){//not working, use simulation to precisely measure
	//printf("updateNav");
	/*float currentDIR = 0;
	while(true){
		float Mag = (SensorValue(encoder1) * 4 *PI)/(360);//function for adding the change in inches to current posiiton
		current.Xpos += cos(current.deg)*(180/PI)*Mag;//cosine of angle times magnitude RADIANS(vector trig)
		current.Ypos += sin(current.deg)*(180/PI)*Mag;//sine of angle times magnitude RADIANS(vector trig)
		current.deg = gyroGet(gyroscope)/10;//gyro reads from0 to 3599, thus 0 to 360 times 10
		currentDIR += Mag;
		encoderReset(encoder1);//resets the quad encoder to have a noticible change in encoder values, rather than always adding a large value
	//	gyroReset(gyroscope);//resets the gyro to refresh with multiple small changes in rotation values
		//delay(25);//refresh every 25ms
	}*/
}//function for calculating the current position of the robot baed off basic vector algebra and trig
void debug(){
	//printf("%d", encoderGet(encoder1));
	printf("%d", analogRead(CBarPot));
	//printf("%d", ultrasonicGet(Usonic));
	//printf("%d", gyroGet(gyroscope));
	//printf("%f", velocity);
	printf("\n");
}//function for debugging sensor values and outputs to text terminal
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
	else return;
}//function for controlling the position of the mobile goal intake

void ClawIntake(){//real noice rn
	if(U7 == 1 || D7 == 1){
		if(U7 == 1 ) motorSet(Claw, 127);
		if (D7 == 1) motorSet(Claw, -127);
	}
	else motorSet(Claw, 0);
}//function for controlling the position of the mobile goal intake
void DannyLiftPID(){
	/*bool MAX = (encoderGet(encoder1) >= 600);
	bool MIN = (encoderGet(encoder1) <= 10);
	///POTENTIOMETER: analogRead(potentiometer)
	if(U6 == 1 || D6 == 1){
		PID.armGoalIsSet = false;
		PID.isRunning = false;
		if(U6 == 1 && !MAX) lift(127);
		if (D6 == 1 && !MIN) lift(-127);
	}
	else{
		if(!PID.armGoalIsSet){
			PID.requestedValue = PIDSensorType;
			PID.armGoalIsSet = true;
		}
		PID.isRunning = true;
	}*/
	//delay(10);
}//function fot the danny lift but with PID implementation {ignore for now}
void DannyLift(){
//basic lift control
	if(U6 == 1 || D6 == 1){
		if(U6 == 1 ) lift(127);
		if (D6 == 1) lift(-127);
	}
	else lift(0);
	//delay(10);
}//function for basic lift control via danny lift
void CBar(bool manual){
	if(manual) manualLiftControl(500, 2500, analogRead(CBarPot), ChainBar, U8, D8, false, false);
	else {
		PID(CBarPot, 2200, ChainBar, 30, 0.075, false, false);//need to insert WHICH sensor to read, not the value (cuz loop == constant)
		if(U8 == 1 || D8 == 1) {manual = true;}
	}
}//function for basic lift control via chain bar
void intake(){//for pneumatics
    if(U8 == 1) toggle = !toggle;
    if(toggle) digitalWrite(3, HIGH);
    else digitalWrite(3, LOW);
    delay(200);
}//function for current intake (for pneumatic claw)
void operatorControl(){//initializes everythin
	initializeOpControl();
	//PID.isRunning = false;
	//PID.requestedValue = PIDSensorType;
	//startTask(updateNav);

	TaskHandle PIDTask = taskCreate(pidController, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	TaskHandle SlewRateMotorTask = taskCreate(MotorSlewRateTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	TaskHandle Speedometer = taskCreate(MeasureSpeed, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);

	taskResume(Speedometer);
	taskResume(SlewRateMotorTask);
//	taskResume(PIDTask);
	while(true){
		debug();
		drive();
		if(R8 == 1) CBar(false);
		//intake();
	//	if(R8 == 1) driveFor(35);
		//if(R7 == 1) driveFor(40);//max can precicely drive is 50
		ClawIntake();
		MobileGoal(true);
		DannyLift();
		delay(3);
	}
}//function for operator control
