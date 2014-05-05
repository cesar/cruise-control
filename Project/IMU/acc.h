#ifndef ACC_H
#define ACC_H

#ifdef __cplusplus
extern "C"
{
#endif

#define ACC_SLAVE_ADDRESS 0x53

void ACCinit();
void ACCReadData(float *x, float *y, float *z);
 

#ifdef __cplusplus
}
#endif

#endif 