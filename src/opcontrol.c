#include "main.h"
#include <math.h>
#include "PORTS.h"

struct robot{
	float Xpos;
	float Ypos;
	float deg;
};
struct robot current;

struct maintainPosition{
	int threshold;
	float kP;
	float armGoal;
	bool armGoalIsSet;
	int LiftMax;
	int LiftMin;
};
volatile struct maintainPosition PID;

#define PI 3.1415926535898628
#define MOTOR_AMOUNT 4

//const in c means only READ ONLY
// Array for requested speed of motors
int motorSlew[MOTOR_AMOUNT];//for each motor
// Array to hold change values for each motor
int SlewAmount[MOTOR_AMOUNT];//for each motor

Gyro gyroscope;
Encoder encoder1;

void initializeOpControl(){
	PID.armGoal = analogRead(potentiometer); // bottom left
	PID.threshold = 7;
	PID.kP = 0.09;
	PID.armGoalIsSet = false;
	PID.LiftMax = 660;
	PID.LiftMin = 1950;
	//gyroShutdown(gyroscope);//resets gyro sensor, rly sketchy
	//gyroscope = gyroInit(1,0);
	current.Xpos = 0.0;
	current.Ypos = 0.0;
	current.deg = 0.0;
}

float avg(float val1, float val2){return (val1 + val2 )	* 0.5;}//func for avg
int sign(int check){
	if (check < 0){return -1;}
	else if (check > 0){return 1;}
	else return 0;
}
void MotorSlewRateTask( void * parameters){//slew rate task
	printf("slewRate");
	int motorI;//which motor are we talking about? INDEX
	for(motorI = 0; motorI < MOTOR_AMOUNT; motorI++){//resets for each motor
		motorSlew[motorI] = 0;//for each motor, the requested speed is 0
		SlewAmount[motorI] = 10;//allow the initial slew change to be 15 (anything else and the motor basically wont move)
	}
	while(true){// run loop for every motor
		for( motorI=1; motorI<MOTOR_AMOUNT; motorI++){
			int POWER = motorGet(motorI);
			//motorCurrent = motor[ motorIndex ];
			if( POWER != motorSlew[motorI] ){//current motor value not equal to goal
				if( POWER < motorSlew[motorI]){//if less than goal
					POWER+=SlewAmount[motorI];//increase by slew amount
					if( POWER >= motorSlew[motorI]){//if equal to or surpassed goal
						motorSet(motorI, motorSlew[motorI]);//sets change to goal
					}
				}
				if( POWER > motorSlew[motorI]){//if currently more than requested
					POWER-=SlewAmount[motorI];//decrease by specific amount
					if(POWER <= motorSlew[motorI]){//once reaches or passes goal
						motorSet(motorI, motorSlew[motorI]);//sets change to goal
					}
				}
				motorSet(motorI, POWER);
			}
		}
		delay(20);//delay 15ms
	}
}

int TruSpeed(float value){
	//printf("truspeed");
	//for quadratic; visit: goo.gl/mhvbx4
	float TrulySped = (value*value*value) / ((127)*(127));
	return(TrulySped);
}

void drive(){
	//printf("drive");
	motorSlew[RightM] = -1*TruSpeed(joystickGetAnalog(1,2));//y axis for baseRight joystick
	//motorSlew[RightM] = -1*TruSpeed(joystickGetAnalog(1,2));//y axis for right joystick
	//motorSlew[LeftM] = TruSpeed(joystickGetAnalog(1,3));//y axis for left  joystick
	motorSlew[LeftM] = TruSpeed(joystickGetAnalog(1,3));//y acis for left  joystick
	delay(5);
}
void lift(int speed){
	//printf("lift");
	motorSlew[LiftM] = speed;
}

void driveFor(float goal){
	//printf("driveFor");
	volatile float distanceTrav = 0.0;//amount of distance travelled
	while ( true ){
		distanceTrav += ( SensorValue ( encoder1 ) * 4 * PI ) / ( 360 );
		if(abs(distanceTrav/goal) < 0.9){
			motorSlew[testM] = 127;//abs( goal - distanceTrav ) * 2;
		}
		else{
			motorSlew[testM] = 0;
		}
		delay(15);
	}
}
void keepArmInPosition( void * parameters){
	while(true){
		if(U6 == 0 && D6 == 0){
			if(abs(analogRead(potentiometer) - PID.armGoal) > PID.threshold){//greater than threshold of 20 LIFTentiometer ticks
				lift((analogRead(potentiometer) - PID.armGoal) * PID.kP);//lifts by the error value, stays at 127 if too large, ises constant scale factor of 0.5 to change when the deceleration process begins, and by how much the deceleration increases
			}
			else {//within threshold
				lift(0);//stops lift motors
			}
			delay(25);
		}
	}
	return;
}

void updateNav(){
	//printf("updateNav");
	float currentDIR = 0;
	while(true){
		float Mag = (SensorValue(encoder1) * 4 *PI)/(360);//function for adding the change in inches to current posiiton

		current.Xpos += cos(current.deg)*(180/PI)*Mag;//cosine of angle times magnitude RADIANS(vector trig)
		current.Ypos += sin(current.deg)*(180/PI)*Mag;//sine of angle times magnitude RADIANS(vector trig)
		current.deg += gyroGet(gyroscope)/10;//gyro reads from0 to 3599, thus 0 to 360 times 10
		currentDIR += Mag;
		//debug
		/*writeDebugStream("%f", currentDIR/140.5);
		writeDebugStreamLine("");*/
		//end_debug
		encoderReset(encoder1);//resets the quad encoder to have a noticible change in encoder values, rather than always adding a large value
		gyroReset(gyroscope);//resets the gyro to refresh with multiple small changes in rotation values
		delay(25);//refresh every 25ms
	}
}
void debug(){
	//printf("%f", ((encoderGet(encoder1) * 4 *PI)/(360)));
	//printf("%d", encoderGet(encoder1));

	printf("%d", U6);
	printf("%d", D6);
	printf("\n");
}
void operatorControl(){//initializes everythin
	initializeOpControl();

	TaskHandle SlewRateMotorTask = taskCreate(MotorSlewRateTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	//startTask(updateNav);
	TaskHandle PIDTask = taskCreate(keepArmInPosition, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);

	taskResume(SlewRateMotorTask);
	taskResume(PIDTask);
	while(true){
		drive();
		if(U5 == 1){driveFor(140.5);}//should do ~11.2

		//bool MAX = (SensorValue[LIFT] < LiftMax);
		//bool MIN = (SensorValue[LIFT] > LiftMin);
		if(U6 == 1 && D6 == 1){
			motorSlew[LiftM] = 0;
		}
		else if(U6 == 1 ){//&& !MAX){
			motorSlew[LiftM] = 127;//abs(analogRead(potentiometer) - PID.LiftMax )* 3);//change by the difference between its curent position and the maximum (slower at the top, fast when not near the top)
			PID.armGoalIsSet = false;
		}
		else if (D6 == 1){// && !MIN){
			motorSlew[LiftM] = -127;//-1 * abs(analogRead(potentiometer) - PID.LiftMin) * 0.5);	//change by the difference betwee the curent position and the bottom, slower at the bottom, while faster when not near the bottom
			PID.armGoalIsSet = false;
		}
		else{
			if(!PID.armGoalIsSet){//recently changed armGoalIsSet
				PID.armGoal = analogRead(potentiometer);
				PID.armGoalIsSet = true;
			}
		}
		delay(10);
	}
}
