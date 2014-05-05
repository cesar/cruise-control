#ifndef GYRO_H
#define GYRO_H

#ifdef __cplusplus
extern "C"
{
#endif

#define GYRO_SLAVE_ADDRESS 0x68


void GyroReadData(int *gyroX, int *gyroY, int *gyroZ);
void GyroRead(float *gyroX, float *gyroY, float *gyroZ);
int gyroReset();
float GyroGetX();
float GyroGetY();
float GyroGetZ();  

#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_ADC_H__