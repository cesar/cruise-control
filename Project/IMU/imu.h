#ifndef IMU_H
#define IMU_H

#ifdef __cplusplus
extern "C"
{
#endif

void IMUinit();
void getValues(float * values);
void getQ(float * q);
void getEuler(float * angles);
void getAngles(float * angles);
void getYawPitchRoll(float * ypr);
float invSqrt(float x);

#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_ADC_H__