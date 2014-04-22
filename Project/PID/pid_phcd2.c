#include <stdio.h>
#include <stdint.h>
#include <time.h>
double setpoint_velocity;
double dt = 100.00;
typedef struct{


	double iState; //Integrator State
	double prev_error;
	double dt;


	//Maximum and minimum allowable integrator state
	double iGain, pGain, dGain;
} SPid;


double UpdatePID(SPid *pid, double error)
{

	double pTerm, dTerm, iTerm;

	//Proportional Term
	pTerm = (pid ->pGain)*error;

	//Update integral state
	pid -> iState += error * (pid->dt);

	//Integral Term
	iTerm = pid->iGain * pid->iState;

	//Derivative Term
	dTerm = pid->dGain * ((error - pid->prev_error)/dt);

	//Update the current position
	pid->prev_error = error;

	return pTerm + iTerm - dTerm;
}

int main(void)
{


	SPid *pid;

	pid->iGain = 0.01;
	pid->pGain = 0.2;
	pid->dGain = 1;
	pid->iState = 0;
	setpoint_velocity = 50;
	double pv = 48;
	while(1)
	{	
		clock_t start = clock();
		clock_t end = start;
		do
		{

			end = clock();

		}while(((float)(end-start)*1000/CLOCKS_PER_SEC) < 100);

		printf("Time: %f\n", (float)(end-start));
		double output = UpdatePID(pid, setpoint_velocity-pv);
		pv += 0.1;
		printf("PV: %f, PID:  %f\n",pv, output);

	}


}