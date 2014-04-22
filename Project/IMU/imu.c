    #include <stdint.h>

#include <stdbool.h>

#include <math.h>

#include "inc/hw_memmap.h"

#include "inc/hw_types.h"

#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"

#include "driverlib/i2c.h"


uint8_t SLAVE_ADDRESS = 0x1D,  POINTER_REGISTER = 0x00; //Address of the SDA pin on the digital thermometer and the pointer address to the temperature register.
uint32_t DATAx0, DATAx1, DATAy0, DATAy1, DATAz0, DATAz1, resultX;
uint8_t REGx0 = 0x32, REGx1 = 0x33, REGy0 = 0x34, REGy1 = 0x35, REGz0 = 0x36, REGz1 = 0x37;


void setupIMU() {
  //Enable the I2C Module
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

  //Configure SDA and SCL
  GPIOPinConfigure(GPIO_PB2_I2C2SCL);
  GPIOPinConfigure(GPIO_PB3_I2C2SDA);

  //Set PA6 as SCL
  GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);

  //Set PA7 as SDA
  GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

  //Configure Master,
  I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false);

  //Enable Master
  I2CMasterEnable(I2C1_BASE);
}

uint32_t readIMU() {

  //Initiate the write sequence, send out the first register address
  I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, false );
  I2CMasterDataPut(I2C2_BASE, REGx0);
  I2CMasterControl( I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
  while(I2CMasterBusBusy(I2C2_BASE)); //Loop until the bus is no longer busy
  I2CSlaveACKValueSet(SLAVE_ADDRESS, true);

  //Receive the first byte
  I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, true);
  I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
  while(I2CMasterBusBusy(I2C2_BASE));
  DATAx0 = I2CMasterDataGet(I2C2_BASE);
  I2CSlaveACKValueSet(SLAVE_ADDRESS, true);

  //Second Byte;
  I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
  while(I2CMasterBusBusy(I2C2_BASE));
  DATAx1 = I2CMasterDataGet(I2C2_BASE);
  I2CSlaveACKValueSet(SLAVE_ADDRESS, true);

  //End of the transmission
  I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
  while(I2CMasterBusBusy(I2C2_BASE));

  resultX = DATAx0 << 8;
  resultX= resultX | DATAx1;

  return resultX;
}

void setup() {
  Serial.begin(9600);
  setupIMU();
}

void loop() {
  Serial.println(readIMU());
  delay(1000);
}