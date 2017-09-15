#include "main.h"

float TruSpeed(float value){
	//for all other polynomials; visit: goo.gl/mhvbx4
	return(getSign(value)*(value*value) /(127));
}//function for calculating the truSpeed function based off a quadratic polynomial

void drive(bool slew){
	if(slew){
		motorSlew[RightBaseM] = TruSpeed(joystickGetAnalog(1,2));//y axis for baseRight joystick
		motorSlew[LeftBaseM] = TruSpeed(joystickGetAnalog(1,3));//y acis for left  joystick
	}
	//noSLEW
	else{
		motorSet(RightBaseM, TruSpeed(joystickGetAnalog(1,2)));
		motorSet(LeftBaseM, TruSpeed(joystickGetAnalog(1,3)));
	}
}//function for driving the robot

void fwds(int speed){
	motorSet(RightBaseM, speed);
	motorSet(LeftBaseM, speed);
}

void driveFor(float goal){
	encoderReset(encoder1);
	int thresh = 10;//10 ticks
	int StartTime = millis();
	float dP = 2;//multiplier for velocity controller
	while ( abs(goal*circum - encoderGet(encoder1)) > thresh){
		if(millis() - StartTime < abs(goal) * 1000){
			fwds(dP * ( ( goal*circum - encoderGet(encoder1) - velocity ) * (10/goal) ) );//SO GOOD
		}
		else return;//do something fancy for checking stalls
	}
	return;
}
void rot(int speed){
	motorSet(RightBaseM, -speed);
	motorSet(LeftBaseM,   speed);
}
void rotFor(int goal){//very simple, minimal rotation function
	gyroReset(gyroscope);
	int thresh = 2;//4 degrees
	while ( abs(gyroGet(gyroscope) - goal) > thresh){
		rot(gyroGet(gyroscope) - goal);
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
