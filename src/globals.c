#include "main.h"


int FourBarMAX = 2400;
int FourBarMIN = 500;
int LiftMAX = 2200;
int LiftMIN = 800;
int MoGoMAX = 2500;
int MoGoMIN = 900;

float circum = 4 * PI;
float velocity;
float avg(float val1, float val2){
	return (val1 + val2 ) * 0.5;
}//func for avg
float getSign(float check){
	if (check < 0) return -1;
	else if (check > 0) return 1;
	return 0;
}
