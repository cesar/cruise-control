#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"

#include "i2c.h"
#include "acc.h"


#define POWER_CTL 0x2D
#define RESET_CMD 0x08

void ACCinit() {

  //Place the accelerometer in measurement mode
  i2c_write(ACC_SLAVE_ADDRESS, RESET_CMD);
}

void ACCReadData(float *x, float *y, float *z) {

  uint32_t accX, accY, accZ;

  i2c_write(ACC_SLAVE_ADDRESS, 0x32);
  accX = i2c_read(ACC_SLAVE_ADDRESS);

  i2c_write(ACC_SLAVE_ADDRESS, 0x33);
  accX = (accX<<8)|i2c_read(ACC_SLAVE_ADDRESS);

  i2c_write(ACC_SLAVE_ADDRESS, 0x34);
  accY = i2c_read(ACC_SLAVE_ADDRESS);

  i2c_write(ACC_SLAVE_ADDRESS, 0x35);
  accY = (accY<<8)|i2c_read(ACC_SLAVE_ADDRESS);

  i2c_write(ACC_SLAVE_ADDRESS, 0x36);
  accZ = i2c_read(ACC_SLAVE_ADDRESS);

  i2c_write(ACC_SLAVE_ADDRESS, 0x37);
  accZ = (accZ<<8)|i2c_read(ACC_SLAVE_ADDRESS);

  *x = (float) accX;
  *y = (float) accY;
  *z = (float) accZ;

}
