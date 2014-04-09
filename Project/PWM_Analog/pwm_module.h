#ifndef PWM_MODULE_H
#define PWM_MODULE_H

#ifdef __cplusplus
extern "C"
{
#endif

void analog_out_setup()
void analog_out(uint32_t ui32Period, uint32_t duty_cycle);

#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_ADC_H__