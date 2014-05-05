#include <inttypes.h>
#include "MadgwickAHRS.h"

#include "imu.h"
#include "gyro.h"
#include "acc.h"


void IMUinit() {
  ACCinit();
}

//Get the values from the IMU (Gyro and Acc)
void getValues(float * values) { 

  ACCReadData(&values[0], &values[1], &values[2]);
  
  GyroRead(&values[3], &values[4], &values[5]);
  
}


void getQ(float * q) {
  float val[9];
  getValues(val);

  //Gyro values are expressed in deg/sec, the * M_PI/180 will convert it to radians/sec
  MadgwickAHRSupdateIMU(val[3] * M_PI/180, val[4] * M_PI/180, val[5] * M_PI/180, val[0], val[1], val[2]);
  q[0] = q0;
  q[1] = q1;
  q[2] = q2;
  q[3] = q3;
}

// Returns the Euler angles in radians defined with the Aerospace sequence.
// See Sebastian O.H. Madwick report 
// "An efficient orientation filter for inertial and intertial/magnetic sensor arrays" Chapter 2 Quaternion representation
void getEuler(float * angles) {
float q[4]; // quaternion
  
  getQ(q);
  
  angles[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1) * 180/M_PI; // psi
  angles[1] = -asin(2 * q[1] * q[3] + 2 * q[0] * q[2]) * 180/M_PI; // theta
  angles[2] = atan2(2 * q[2] * q[3] - 2 * q[0] * q[1], 2 * q[0] * q[0] + 2 * q[3] * q[3] - 1) * 180/M_PI; // phi
}


void getAngles(float * angles) {
  float a[3]; //Euler
  getEuler(a);

  angles[0] = a[0];
  angles[1] = a[1];
  angles[2] = a[2];
  
  if(angles[0] < 0){
    angles[0] += 360;
  } if(angles[1] < 0){ 
    angles[1] += 360;
  } if(angles[2] < 0)angles[2] += 360;
}

void getYawPitchRoll(float * ypr) {
  float q[4]; // quaternion
  float gx, gy, gz; // estimated gravity direction
  getQ(q);
  
  gx = 2 * (q[1]*q[3] - q[0]*q[2]);
  gy = 2 * (q[0]*q[1] + q[2]*q[3]);
  gz = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
  
  ypr[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1) * 180/M_PI;
  ypr[1] = atan(gx / sqrt(gy*gy + gz*gz))  * 180/M_PI;
  ypr[2] = atan(gy / sqrt(gx*gx + gz*gz))  * 180/M_PI;
}


float invSqrt(float x) {
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long*)&y;
  i = 0x5f3759df - (i>>1);
  y = *(float*)&i;
  y = y * (1.5f - (halfx * y * y));
  return y;
}
