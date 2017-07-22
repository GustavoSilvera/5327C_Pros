#include "main.h"
#include <math.h>
#include "PORTS.h"

//#define PID_SENSOR_SCALE    1

#define PID_MOTOR_SCALE     -1
#define PID_DRIVE_MAX       127
#define PID_DRIVE_MIN     (-127)
#define PID_INTEGRAL_LIMIT  50

struct robot{
	float Xpos;
	float Ypos;
	float deg;
};
struct robot current;

struct maintainPosition{
	bool isRunning, armGoalIsSet;
	int threshold;
	float error, integral, derivative, lastError;
	int currentPos;
	float requestedValue;
	//, kI, kD;
};
volatile struct maintainPosition PID;

#define PI 3.1415926535898628
#define MOTOR_AMOUNT 6
#define PIDSensorType encoderGet(encoder1)
//analogRead(potentiometer)

//const in c means only READ ONLY
// Array for requested speed of motors
int motorSlew[MOTOR_AMOUNT];//for each motor
// Array to hold change values for each motor
int SlewAmount[MOTOR_AMOUNT];//for each motor
volatile bool toggle = false;

Gyro gyroscope;
Encoder encoder1;
Encoder ripperEncoder;
Ultrasonic Usonic;

void initializeOpControl(){
	PID.requestedValue = 200;
	PID.isRunning = true;
	gyroShutdown(gyroscope);//resets gyro sensor, rly sketchy
	gyroscope = gyroInit(1,0);
	current.Xpos = 0.0;
	current.Ypos = 0.0;
	current.deg = 0.0;
}
float avg(float val1, float val2){
	return (val1 + val2 ) * 0.5;
}//func for avg
float getSign(float check){
	if (check < 0) return -1;
	else if (check > 0) return 1;
	else return 0;
}//func for getting sign of a number
void MotorSlewRateTask( void * parameters){//slew rate task
	printf("slewRate");
	int motorI;//which motor are we talking about? INDEX
	//for(motorI = 0; motorI <= MOTOR_AMOUNT; motorI++){//resets for each motor
	//	motorSlew[motorI] = 0;//for each motor, the requested speed is 0
	//	SlewAmount[motorI] = 20;//allow the initial slew change to be 15 (anything else and the motor basically wont move)
	//}//USED FOR INITIALIZING EVERY MOTOR_AMOUNT
	SlewAmount[useless] = 10;//useless motre anyways.
	SlewAmount[RightBaseM] = 25;//lots o' speed
	SlewAmount[LeftBaseM] = 25;//lots o' speed
	SlewAmount[ChainBar] = 20;//speed
	SlewAmount[DannyLiftM] = 20;//want more speed
	SlewAmount[MoGo] = 10;//higher torque
	while(true){// run loop for every motor
		for( motorI = 0; motorI <= MOTOR_AMOUNT; motorI++){
			//motorCurrent = motor[ motorIndex ];
			if( motorGet(motorI) != motorSlew[motorI] ){//current motor value not equal to goal
				if( motorGet(motorI) < motorSlew[motorI]){//if less than goal
					motorSet(motorI, motorGet(motorI)+SlewAmount[motorI]);//decrease by specific amount
					if( motorGet(motorI) >= motorSlew[motorI]){//if equal to or surpassed goal
						motorSet(motorI, motorSlew[motorI]);//sets change to goal
					}
				}
				if( motorGet(motorI) > motorSlew[motorI]){//if currently more than requested
					motorSet(motorI, motorGet(motorI)-SlewAmount[motorI]);//decrease by specific amount
					if(motorGet(motorI) <= motorSlew[motorI]){//once reaches or passes goal
						motorSet(motorI, motorSlew[motorI]);//sets change to goal
					}
				}
				//motorSet(motorI, motorGet(motorI));
			}
		}
		delay(25);//delay 15ms
	}
}//task for "slew"-ing the motres by adding to their power via loops
void lift(float speed){
	motorSlew[DannyLiftM] = speed;
}//function for controlling the danny lift (1 motor y cabled one motor has to be reversed)
void pidController(void * parameters){
	float kP = 0.25;//remove later
	float kI = 15;//remove later
	float kD = 0.0;//remove later
    // If we are using an encoder then clear it
	encoderReset(encoder1);
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
				motorSet(DannyLiftM, power);
			}
    		// Run at 50Hz
		}
		else{
			PID.lastError = 0;
			PID.integral = 0;
			PID.error = 0;
			PID.derivative = 0;
		}
        delay(10);
    }
}//task for a PID lift controller, not implemented well YET, ignore for now
float TruSpeed(float value){
	//for all other polynomials; visit: goo.gl/mhvbx4
	return(getSign(value)*(value*value) /(127));
}//function for calculating the truSpeed function based off a quadratic polynomial
void drive(){
	motorSlew[RightBaseM] = -1*TruSpeed(joystickGetAnalog(1,2));//y axis for baseRight joystick
	motorSlew[LeftBaseM] = TruSpeed(joystickGetAnalog(1,3));//y acis for left  joystick
	delay(10);
}//function for driving the robot
/*void driveFor(float goal){
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
}*/
//ignore// function for driving the robot a little bit
void updateNav(){//not working, use simulation to precisely measure
	//printf("updateNav");
	float currentDIR = 0;
	while(true){
		float Mag = (SensorValue(encoder1) * 4 *PI)/(360);//function for adding the change in inches to current posiiton
		current.Xpos += cos(current.deg)*(180/PI)*Mag;//cosine of angle times magnitude RADIANS(vector trig)
		current.Ypos += sin(current.deg)*(180/PI)*Mag;//sine of angle times magnitude RADIANS(vector trig)
		current.deg = gyroGet(gyroscope)/10;//gyro reads from0 to 3599, thus 0 to 360 times 10
		currentDIR += Mag;
		encoderReset(encoder1);//resets the quad encoder to have a noticible change in encoder values, rather than always adding a large value
	//	gyroReset(gyroscope);//resets the gyro to refresh with multiple small changes in rotation values
		delay(25);//refresh every 25ms
	}
}//function for calculating the current position of the robot baed off basic vector algebra and trig
void debug(){
	printf("%d", encoderGet(encoder1));
	//printf("%f", analogRead(potentiometer));
	//printf("%d", ultrasonicGet(Usonic));
	//printf("%d", gyroGet(gyroscope));
	//printf("%d", D6);
	printf("\n");
}//function for debugging sensor values and outputs to text terminal
void MobileGoal(){
	if(U8 == 1)	motorSlew[MoGo] = 127;
	else if (D8 == 1) motorSlew[MoGo] = -127;
	else if (D8 == 0 && U8 == 0) motorSlew[MoGo] = 0;
}//function for controlling the position of the mobile goal intake
void DannyLiftPID(){
	bool MAX = (encoderGet(encoder1) >= 600);
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
	}
	delay(10);
}//function fot the danny lift but with PID implementation {ignore for now}
void DannyLift(){
//basic lift control
	if(U6 == 1 || D6 == 1){
		if(U6 == 1 ) lift(127);
		if (D6 == 1) lift(-127);
	}
	else lift(0);
	delay(10);
}//function for basic lift control via danny lift
void chainBarLift(){
	if(U5 == 1 || D5 == 1){
		if(U5 == 1 ) motorSet(ChainBar, 127);//[ChainBar] = 127; }
		if (D5 == 1) motorSet(ChainBar, -127);
	}
	else motorSlew[ChainBar] = 0;
	delay(10);
}//function for controlling the chain bar atop the danny lift (similar structure)
void intake(){
    if(U8 == 1) toggle = !toggle;
    if(toggle) digitalWrite(3, HIGH);
    else digitalWrite(3, LOW);
    delay(200);
}//function for current intake (as of rn is pneumatic claw)
void operatorControl(){//initializes everythin
	initializeOpControl();
	PID.isRunning = false;
	PID.requestedValue = PIDSensorType;
	TaskHandle SlewRateMotorTask = taskCreate(MotorSlewRateTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	//startTask(updateNav);
	TaskHandle PIDTask = taskCreate(pidController, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	taskResume(SlewRateMotorTask);
	taskResume(PIDTask);
	while(true){
		debug();
		drive();
		//intake();
		MobileGoal();
		DannyLiftPID();
		chainBarLift();
		delay(10);
	}
}//function for operator control
