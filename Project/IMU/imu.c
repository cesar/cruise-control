#include <stdint.h>

#include <stdbool.h>

#include <math.h>

#include "driverlib/pin_map.h"

#include "inc/hw_memmap.h"

#include "inc/hw_types.h"

#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"

#include "driverlib/i2c.h"


#define GPIO_PE4_I2C2SCL  0x00041003

#define GPIO_PE5_I2C2SDA  0x00041403

#define MEASURE_MODE 0x08

#define POWER_CTL 0x2D

void write(uint8_t regAddr, uint8_t data);
uint8_t read(uint8_t regAddr);

uint8_t SLAVE_ADDRESS = 0x53; 
uint32_t DATAx, DATAy, DATAz;
uint8_t REGx0 = 0x32, REGx1 = 0x33, REGy0 = 0x34, REGy1 = 0x35, REGz0 = 0x36, REGz1 = 0x37;


void setupIMU() {
  //Enable the I2C Module
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

  //Configure SDA and SCL
  GPIOPinConfigure(GPIO_PE4_I2C2SCL);
  GPIOPinConfigure(GPIO_PE5_I2C2SDA);

  //Set PE4 as SCL
  GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);

  //Set PE5 as SDA
  GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);

  //Configure Master,
  I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false);

  //Enable Master
  I2CMasterEnable(I2C2_BASE);

  //Set the IMU to measure mode
  write(POWER_CTL, MEASURE_MODE);

}

uint32_t readIMU() {

  // DATAx = read(REGx0);
  // DATAx = DATAx << 8;
  // DATAx = DATAx | read(REGx1);
  DATAx = read(0x39);

  return DATAx;
}

void write(uint8_t regAddr, uint8_t data) {

  //Set the register address that will be altered
  I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, false);
  I2CMasterDataPut(I2C2_BASE, regAddr);
  I2CMasterControl( I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
  while(I2CMasterBusBusy(I2C2_BASE)); //Loop until the bus is no longer busy

  //Send the actual data to the IMU
  I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, false);
  I2CMasterDataPut(I2C2_BASE, data);
  I2CMasterControl( I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
  while(I2CMasterBusBusy(I2C2_BASE)); //Loop until the bus is no longer busy

}

uint8_t read(uint8_t regAddr) {

  //Set the register address that will be altered
  I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, false);
  I2CMasterDataPut(I2C2_BASE, regAddr);
  I2CMasterControl( I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
  while(I2CMasterBusBusy(I2C2_BASE)); //Loop until the bus is no longer busy

  //Receive the first byte
  I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, true);
  I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
  while(I2CMasterBusBusy(I2C2_BASE));
  uint8_t temp = I2CMasterDataGet(I2C2_BASE);
  I2CSlaveACKValueSet(SLAVE_ADDRESS, true);
  return temp;
}



void setup() {
  Serial.begin(9600);
  setupIMU();
}

void loop() {
  Serial.println(readIMU());
  delay(1000);
}