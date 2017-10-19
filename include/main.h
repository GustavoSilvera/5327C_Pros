/** @file main.h
 * @brief Header file for global functions
 *
 * Any experienced C or C++ programmer knows the importance of header files. For those who
 * do not, a header file allows multiple files to reference functions in other files without
 * necessarily having to see the code (and therefore causing a multiple definition). To make
 * a function in "opcontrol.c", "auto.c", "main.c", or any other C file visible to the core
 * implementation files, prototype it here.
 *
 * This file is included by default in the predefined stubs in each VEX Cortex PROS Project.
 *
 * Copyright (c) 2011-2016, Purdue University ACM SIGBots.
 * All rights reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#ifndef MAIN_H_

// This prevents multiple inclusion, which isn't bad for this file but is good practice
#define MAIN_H_
#include <API.h>
#include "globals.h"
#include "slewRate.h"
#include "PID.h"
#include "base.h"
#include "measure.h"
#include "lift.h"
// Allow usage of this file in C++ programs
#ifdef __cplusplus
extern "C" {
#endif

// A function prototype looks exactly like its declaration, but with a semicolon instead of
// actual code. If a function does not match a prototype, compile errors will occur.

// Prototypes for initialization, operator control and autonomous

/**
 * Runs the user autonomous code. This function will be started in its own task with the default
 * priority and stack size whenever the robot is enabled via the Field Management System or the
 * VEX Competition Switch in the autonomous mode. If the robot is disabled or communications is
 * lost, the autonomous task will be stopped by the kernel. Re-enabling the robot will restart
 * the task, not re-start it from where it left off.
 *
 * Code running in the autonomous task cannot access information from the VEX Joystick. However,
 * the autonomous function can be invoked from another task if a VEX Competition Switch is not
 * available, and it can access joystick information if called in this way.
 *
 * The autonomous task may exit, unlike operatorControl() which should never exit. If it does
 * so, the robot will await a switch to another mode or disable/enable cycle.
 */
void autonomous();
/**
 * Runs pre-initialization code. This function will be started in kernel mode one time while the
 * VEX Cortex is starting up. As the scheduler is still paused, most API functions will fail.
 *
 * The purpose of this function is solely to set the default pin modes (pinMode()) and port
 * states (digitalWrite()) of limit switches, push buttons, and solenoids. It can also safely

 * configure a UART port (usartOpen()) but cannot set up an LCD (lcdInit()).
 */
void initializeIO();
/**
 * Runs user initialization code. This function will be started in its own task with the default
 * priority and stack size once when the robot is starting up. It is possible that the VEXnet
 * communication link may not be fully established at this time, so reading from the VEX
 * Joystick may fail.
 *
 * This function should initialize most sensors (gyro, encoders, ultrasonics), LCDs, global
 * variables, and IMEs.
 *
 * This function must exit relatively promptly, or the operatorControl() and autonomous() tasks
 * will not start. An autonomous mode selection menu like the pre_auton() in other environments
 * can be implemented in this task if desired.
 */
void initialize();
/**
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
 void operatorControl();

//joysticks: parameters are (1[idek], vexRT[channel #])

#define JRightX	joystickGetAnalog(1,1)
#define JRightY	joystickGetAnalog(1,2)
#define JLeftX 	joystickGetAnalog(1,4)
#define JLeftY	joystickGetAnalog(1,3)

//PARTNER JOYSTICKer
    #define JRightX2	joystickGetAnalog(2,1)
    #define JRightY2	joystickGetAnalog(2,2)
    #define JLeftX2 	joystickGetAnalog(2,4)
    #define JLeftY2 	joystickGetAnalog(2,3)

enum {//motors
    useless = 1,//port1 (DONT EVER USE)
    LeftBaseYCable = 2,//left base motors
    LeftBase = 3,//left base motors
    FourBarYCable = 4,//lift left motor
    //no 5th motre
    liftYCable = 6,//claw control
    MoGoYCable = 7,//right base motors
    RightBaseYCable = 8,//lift right motors
    RightBase = 9,//chain bar
};
//#define gyroscope 1
extern Gyro gyroscope;
extern Encoder encoder1;
extern Encoder liftEncoder;
extern Ultrasonic Usonic;

#define MoGoPot 2
#define FourBarPot 3
//#define LiftPot 4
#define button 8//digital button port



#define U5    	joystickGetDigital(1,5,JOY_UP)//5U
#define D5	    joystickGetDigital(1,5,JOY_DOWN)//5D

#define U6  	joystickGetDigital(1,6,JOY_UP)//6U
#define D6	    joystickGetDigital(1,6,JOY_DOWN)//6D

#define U7  	joystickGetDigital(1,7,JOY_UP)//7U
#define D7	    joystickGetDigital(1,7,JOY_DOWN)//7D
#define L7    	joystickGetDigital(1,7,JOY_LEFT)//7L
#define R7	    joystickGetDigital(1,7,JOY_RIGHT)//7R

#define U8  	joystickGetDigital(1,8,JOY_UP)//8U
#define D8	    joystickGetDigital(1,8,JOY_DOWN)//8D
#define L8    	joystickGetDigital(1,8,JOY_LEFT)//8L
#define R8	    joystickGetDigital(1,8,JOY_RIGHT)//8R

//PARTNER JOYSTICK

#define U52    	joystickGetDigital(2,5,JOY_UP)//5U
#define D52	    joystickGetDigital(2,5,JOY_DOWN)//5D

#define U62  	joystickGetDigital(2,6,JOY_UP)//6U
#define D62	    joystickGetDigital(2,6,JOY_DOWN)//6D

#define U72  	joystickGetDigital(2,7,JOY_UP)//7U
#define D72	    joystickGetDigital(2,7,JOY_DOWN)//7D
#define L72    	joystickGetDigital(2,7,JOY_LEFT)//7L
#define R72	    joystickGetDigital(2,7,JOY_RIGHT)//7R

#define U82  	joystickGetDigital(2,8,JOY_UP)//8U
#define D82	    joystickGetDigital(2,8,JOY_DOWN)//8D
#define L82    	joystickGetDigital(2,8,JOY_LEFT)//8L
#define R82	    joystickGetDigital(2,8,JOY_RIGHT)//8R

// End C++ export structure
#ifdef __cplusplus
}
#endif

#endif
