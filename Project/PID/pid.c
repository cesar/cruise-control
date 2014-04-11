#include <stdio.h>
#include <stdint.h>

typedef struct{


	double dState; //Last velocity Input
	double iState; //Integrator State
	double iMax, iMin;
	double setVelocity;

	double iGain, //Integral gain
			pGain, //Proportional Gain
			dGain; //Derivative gain

} SPid;
double UpdatePID(SPid * pid, double velocity, double inclination)
{

	double error = (pid -> setVelocity) - velocity;


	double pTerm,
			dTerm,
			iTerm;
	//Calculate Proportional term
	pTerm = pid ->pGain*error;

	//Calculate the integral state with appropriate limiting
	// pid -> iState += error;
	// if(pid->iState > pid -> iMax) pid -> iState = pid -> iMax;
	// else if(pid -> iState < pid -> iMin)
	// {
	// 	pid -> iState = pid -> iMin;
	// }

	iTerm = pid -> iGain * iState; //Calculate Integral term
	dTerm = pid -> dGain * (velocity -pid -> dState); //Calculate derivative Term
	pid -> dState = velocity; //Current speed
	return pTerm + iTerm - dTerm;
}