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
    
    pid.iGain = 3.15; //
    pid.pGain = 0.63; //
    pid.dGain = 0.315; //
    pid.iState = 0;
    pid.pre_error = 0;

    Serial.begin(9600);
    Serial.println("PID Control:");
	setpoint_velocity = 50.00;
	float PV = 0.00;

	int f_counter =0;
	char data_out[10];
	while(f_counter <80)
	{
        
		if(f_counter == 20){ PV = 47.00;}
		if(f_counter == 40){ PV = 55.00;}
				if(f_counter == 60){ setpoint_velocity = 40;}




		sprintf(data_out, "%f , %f",(f_counter * 100.00), PV);
        Serial.println(data_out);
		float output = UpdatePID(&pid, setpoint_velocity-PV, 0.1);
   		PV += (output);
		f_counter++;
		
        
	}
    
}

void loop(){}