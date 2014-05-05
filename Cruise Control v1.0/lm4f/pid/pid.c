#include <stdio.h>
#include <stdint.h>
#include "pid.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

float setpoint_velocity;

float UpdatePID(SPid *pid, float error, float dt)
{
	float pTerm, dTerm, iTerm;
    
	//Proportional Term
	pTerm = (pid ->pGain)*error;
    
	//Update integral state
	pid -> iState += error*dt;
    
	// if(pid->iState > pid->iMax) pid ->iState = pid->iMax;
	// else if(pid->iState < pid->iMin) pid->iState=pid->iMin;
	iTerm = pid ->iGain * pid->iState;
    
	dTerm = pid->dGain *(error -pid->pre_error)/ dt;
	
	//Update the current position
	pid->pre_error = error;
	SysCtlDelay((SysCtlClockGet() * 0.1)/3);
	return pTerm + iTerm + dTerm;
}



void setup_PID(SPid *pid)
{	
   
    pid->iGain = 3.15;
    pid->pGain = 0.63;
    pid->dGain = 0.315; 
    pid->iState = 0;
    pid->pre_error = 0;
    
}

