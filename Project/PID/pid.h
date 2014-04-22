

#ifndef PID_H
#define PID_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    
    
	float iState; //Integrator State
	float pre_error;
	float iGain, pGain, dGain;
} SPid;

#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_ADC_H__