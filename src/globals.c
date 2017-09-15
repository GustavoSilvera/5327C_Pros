#include "main.h"


int CBarMAX = 4000;
int CBarMIN = 100;
int DannyMAX = 2800;
int DannyMIN = 600;
int MoGoMAX = 2200;
int MoGoMIN = 800;




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
