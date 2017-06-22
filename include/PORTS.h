#ifndef _PORTS_H_
#define _PORTS_H_

// MOTOR DEFINITIONS
/*
    #define SensorValue encoderGet
    extern Encoder quad1;
    //quad1 encoderInit (1,2,false)
    //joysticks: parameters are (1[idek], vexRT[channel #])

    #define JRightX	joystickGetAnalog(1,1)
    #define JRightY	joystickGetAnalog(1,2)
    #define JLeftX 	joystickGetAnalog(1,4)
    #define JLeftY	joystickGetAnalog(1,3)
    //or if want simpler porting from robotc code
    #define vexRTCh1	joystickGetAnalog(1,1)
    #define vexRTCh2	joystickGetAnalog(1,2)
    #define vexRTCh3	joystickGetAnalog(1,3)
    #define vexRTCh4	joystickGetAnalog(1,4)

    enum {
        useless = 1,//port1 (DONT EVER USE)
    	testM,//port2
    	RightM,//port3
    	LeftM,//port4
    	LiftM//port5
    };
	#define gyro 1
	#define potentiometer 2
	Gyro gyro;

    #define U6  	joystickGetDigital(1,6,JOY_UP)//6U
    #define D6	    joystickGetDigital(1,6,JOY_DOWN)//6D
    #define U5    	joystickGetDigital(1,5,JOY_UP)//5U
    #define D5	    joystickGetDigital(1,5,JOY_DOWN)//5D

    #define U8  	joystickGetDigital(1,8,JOY_UP)//8U
    #define D8	    joystickGetDigital(1,8,JOY_DOWN)//8D
    #define L8    	joystickGetDigital(1,8,JOY_LEFT)//8L
    #define R8	    joystickGetDigital(1,8,JOY_Right)//8R

    #define U7  	joystickGetDigital(1,7,JOY_UP)//7U
    #define D7	    joystickGetDigital(1,7,JOY_DOWN)//7D
    #define L7    	joystickGetDigital(1,7,JOY_LEFT)//7L
    #define R7	    joystickGetDigital(1,7,JOY_Right)//7R
*/
#endif /* end of include guard: _PORTS_H_ */
