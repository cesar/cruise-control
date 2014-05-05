#include "i2c.h"
#include "gyro.h"

void GyroReadData(int *gyroX, int *gyroY, int *gyroZ) {

  i2c_write(GYRO_SLAVE_ADDRESS, 0x1D);
  gyroX = i2c_read(GYRO_SLAVE_ADDRESS);

  i2c_write(GYRO_SLAVE_ADDRESS, 0x1E);
  gyroX = (gyroX<<8)|i2c_read(GYRO_SLAVE_ADDRESS);

  i2c_write(GYRO_SLAVE_ADDRESS, 0x1F);
  gyroY = i2c_read(GYRO_SLAVE_ADDRESS);

  i2c_write(GYRO_SLAVE_ADDRESS, 0x20);
  gyroY = (gyroY<<8)|i2c_read(GYRO_SLAVE_ADDRESS);

  i2c_write(GYRO_SLAVE_ADDRESS, 0x21);
  gyroZ = i2c_read(GYRO_SLAVE_ADDRESS);

  i2c_write(GYRO_SLAVE_ADDRESS, 0x22);
  gyroZ = (gyroZ<<8)|i2c_read(GYRO_SLAVE_ADDRESS);

}

void GyroRead(float *gyroX, float *gyroY, float *gyroZ) {
  int x, y, z;

  GyroReadData(&x, &y, &z);

  *gyroX = (float) x / 14.375;
  *gyroy = (float) y / 14.375;
  *gyroZ = (float) z / 14.375;
}

int gyroReset() {

  //Select the reset register within the Gyro
  i2c_write(GYRO_SLAVE_ADDRESS, 0x3E);

  //Write the reset command
  i2c_write(GYRO_SLAVE_ADDRESS, 0x80);

}

float GyroGetX() {
  return (float) x / 14.375 ;
}

float GyroGetY() {
  return (float) y / 14.375;
}

float GyroGetZ() {
  return (float) z / 14.375 ; 
}