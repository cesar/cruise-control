#include <stdio.h>
#include <stdint.h>
#include <pid.h>

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



void setup()
{	


	SPid pid;
    
    // pid = (SPid){ .iGain = 0, .pGain = 2.3, .dGain = 0, .iState = 0, .pre_error = 0};
    pid.iGain = 0;
    pid.pGain = 1.3;
    pid.dGain = 0;
    pid.iState = 0;
    pid.pre_error = 0;

    Serial.begin(9600);
    Serial.println("PID Control:");
	setpoint_velocity = 50.00;
	float PV = 0.00;

	int f_counter =0;
	char data_out[10];
	while(f_counter <300)
	{
        
		if(f_counter > 150 && (int)PV == 50) PV = 47.00;
		sprintf(data_out, "%f , %f",(f_counter * 100.00), PV);
        Serial.println(data_out);
		float output = UpdatePID(&pid, setpoint_velocity-PV, 100);
   		PV += output;

		

		f_counter++;
		
        
	}
    
}

void loop(){}