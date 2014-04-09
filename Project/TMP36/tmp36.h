#ifndef TMP36_H
#define TMP36_H

#ifdef __cplusplus
extern "C"
{
#endif

void setup_tmp36();
int get_analog_temp();
void disable_module();
void enable_module();
#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_ADC_H__