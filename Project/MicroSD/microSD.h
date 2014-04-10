#ifndef microSD_H
#define microSD_H

#ifdef __cplusplus
extern "C"
{
#endif


void setup_microSD();
void close();
void open_statuslog();
void open_datalog();
void write_datalog(char *velocity, char *coordX, char *coordY, char *time_log, char *ambient_temp, char *motor_temp);
void write_statuslog();
#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_ADC_H__