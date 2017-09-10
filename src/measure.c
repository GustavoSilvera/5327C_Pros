#include "main.h"

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
