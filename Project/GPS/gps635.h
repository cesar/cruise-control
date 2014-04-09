#ifndef GPS635_H
#define GPS635_H

#ifdef __cplusplus
extern "C"
{
#endif

void coldStartReset();
void hotStartReset();
void enableSBAS();
void disableSBAS();
void disableRMC();
void disableGGA();
void disableGLL();
void disableGSA();
void disableGSV();
void disableVTG();
//Program flow commands
char *getTime();
void enable_GPS();
void setup_GPS();
void parseTime();
void listen_GPS();

char *getVelocity();
void parseVelocity();

void genericReader();
#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_ADC_H__