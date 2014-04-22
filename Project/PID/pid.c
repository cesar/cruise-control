#include <stdio.h>
#include <stdint.h>

typedef struct{


	double dState; //Last velocity Input
	double iState; //Integrator State
	double iMax, iMin;
	double setVelocity;
	double previous_error;
	double dt; //Time Change


	double iGain, //Integral gain
			pGain, //Proportional Gain
			dGain; //Derivative gain

} SPid;

double UpdatePID(SPid * pid, double process_velocity, double inclination)
{

	double error = (pid -> setVelocity) - process_velocity;


	double proportional_term, integral_term, derivative_term;
	//Calculate Proportional term

	//Calculate the integral state with appropriate limiting
	// pid -> iState += error;
	// if(pid->iState > pid -> iMax) pid -> iState = pid -> iMax;
	// else if(pid -> iState < pid -> iMin)
	// {
	// 	pid -> iState = pid -> iMin;
	// }
	//Calculate proportional term
	proportional_term = pid ->pGain*error;

	//Calculate integral term
	pid->iState += error * pid-> dt;
	integral_term = (pid->iGain) * pid->iState;

	//Calculate derivative term
	derivative_term = (pid ->dGain)*(error - pid-> previous_error)/(pid->dt); //Calculate derivative Term
	
	double output = proportional_term + integral_term + derivative_term;
	pid -> previous_error = error;

	return output;
}