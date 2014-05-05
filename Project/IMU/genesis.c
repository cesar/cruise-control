#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"

#include "IMU/imu.h"

void setup()
{
    Serial.begin(9600);

  IMUinit();

  float values[9];
}

void loop()
{
  getValues(values);

  Serial.println(values[0]);

  delay(1000);
}

