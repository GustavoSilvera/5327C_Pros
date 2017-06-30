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
	bool isRunning;
	int threshold;
	float error, integral, derivative, lastError;
	float currentPos;
	float requestedValue;
	//, kI, kD;
};
volatile struct maintainPosition PID;

#define PI 3.1415926535898628
#define MOTOR_AMOUNT 6
//const in c means only READ ONLY
// Array for requested speed of motors
int motorSlew[MOTOR_AMOUNT];//for each motor
// Array to hold change values for each motor
int SlewAmount[MOTOR_AMOUNT];//for each motor

Gyro gyroscope;
Encoder encoder1;
Encoder ripperEncoder;

void initializeOpControl(){
	//PID.armGoal = analogRead(potentiometer); // bottom left
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
	for(motorI = 0; motorI <= MOTOR_AMOUNT; motorI++){//resets for each motor
		motorSlew[motorI] = 0;//for each motor, the requested speed is 0
		SlewAmount[motorI] = 10;//allow the initial slew change to be 15 (anything else and the motor basically wont move)
	}
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
}
void pidController(void * parameters){
	float kP = 0.2;//remove later
	float kI = 25;//remove later
	float kD = 0.0;//remove later
    // If we are using an encoder then clear it
	encoderReset(ripperEncoder);
    PID.lastError = 0;
    PID.integral = 0;
    while(true){
        if( PID.isRunning ){
            PID.currentPos = digitalRead(ripperEncoder);// * sensorScale;idk if i need this, probs not.
            PID.error = PID.currentPos - PID.requestedValue;//calculate error
            if( kI != 0 ){
                if( abs(PID.error) < PID_INTEGRAL_LIMIT ){
                    PID.integral += PID.error;//used for averaging the integral amount, later in motor power divided by 25
				}
                else{
                    PID.integral = 0;
				}
            }
            else{
                PID.integral = 0;
			}
            // calculate the derivative
            PID.derivative = PID.error - PID.lastError;
            PID.lastError  = PID.error;
            // calculate drive (in this case, just for the chain (ripper) )
			motorSlew[ChainL] = ( (kP * PID.error) + (kI * PID.integral) + (kD * PID.derivative) );
			motorSlew[ChainR] = ( (kP * PID.error) + (PID.integral/kI) + (kD * PID.derivative) );
		}
        else{
            // clear all
            PID.error = 0;
            PID.lastError = 0;
            PID.integral = 0;
            PID.derivative = 0;
        }
        // Run at 50Hz
        delay(25);
    }
}

float TruSpeed(float value){
	//for cubic; visit: goo.gl/mhvbx4
	return(sign(value)*(value*value) /(127));
}

void drive(){
	motorSlew[RightBaseM] = -1*TruSpeed(joystickGetAnalog(1,2));//y axis for baseRight joystick
	motorSlew[LeftBaseM] = TruSpeed(joystickGetAnalog(1,3));//y acis for left  joystick
	delay(10);
}
void lift(int speed){
	motorSlew[ChainL] = -speed;
	motorSlew[ChainR] = speed;
}

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
/*void keepArmInPosition( void * parameters){
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
}*/

void updateNav(){
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
}
void debug(){
	//printf("%f", ((encoderGet(encoder1) * 4 *PI)/(360)));
	//printf("%d", encoderGet(encoder1));
	printf("%d", analogRead(potentiometer2));
	//printf("%d", gyroGet(gyroscope));
	//printf("%d", D6);
	printf("\n");
}
void MobileGoal(){
	if(U5 == 1){
		motorSlew[MoGo] = 127;
	}
	else if (D5 == 1){
		motorSlew[MoGo] = -127;
	}
	else if (D5 == 0 && U5 == 0){
		motorSlew[MoGo] = 0;
	}
}
void ripper(){
	//bool MAX = (SensorValue[LIFT] < LiftMax);
	//bool MIN = (SensorValue[LIFT] > LiftMin);
	if(U6 == 1 && D6 == 1){lift(0);PID.isRunning = false;}
	else if(U6 == 1 ){//&& !MAX){
		PID.isRunning = false;
		lift(127);//abs(analogRead(potentiometer) - PID.LiftMax )* 3);//change by the difference between its curent position and the maximum (slower at the top, fast when not near the top)
		//PID.armGoalIsSet = false;
	}
	else if (D6 == 1){// && !MIN){
		PID.isRunning = false;
		lift(-127);//-1 * abs(analogRead(potentiometer) - PID.LiftMin) * 0.5);	//change by the difference betwee the curent position and the bottom, slower at the bottom, while faster when not near the bottom
		//PID.armGoalIsSet = false;
	}
	else{
		PID.requestedValue = digitalRead(ripperEncoder);
		PID.isRunning = true;
	}
	delay(15);
}
void operatorControl(){//initializes everythin
	initializeOpControl();

	TaskHandle SlewRateMotorTask = taskCreate(MotorSlewRateTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	//startTask(updateNav);
	TaskHandle PIDTask = taskCreate(pidController, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	taskResume(SlewRateMotorTask);
	taskResume(PIDTask);
	while(true){
		debug();
		drive();
		MobileGoal();
		ripper();
		delay(10);
	}
}
