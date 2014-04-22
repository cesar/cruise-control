#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"

#include "tmp102.h"

#define GPIO_PA6_I2C1SCL        0x00001803
#define GPIO_PA7_I2C1SDA        0x00001C03


uint8_t SLAVE_ADDRESS = 0x48,  POINTER_REGISTER = 0x00; //Address of the SDA pin on the digital thermometer and the pointer address to the temperature register.

uint32_t first_byte, second_byte, temperature, result;

void setupTMP102() {
  //Enable the I2C Module
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

  //Configure SDA and SCL
  GPIOPinConfigure(GPIO_PA6_I2C1SCL);
  GPIOPinConfigure(GPIO_PA7_I2C1SDA);

  //Set PA6 as SCL
  GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);

  //Set PA7 as SDA
  GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

  //Configure Master,
  I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);

  //Enable Master
  I2CMasterEnable(I2C1_BASE);

  //Set pointer register to the temperature register
  I2CMasterSlaveAddrSet(I2C1_BASE, SLAVE_ADDRESS, false );
  I2CMasterDataPut(I2C1_BASE, POINTER_REGISTER);
  I2CMasterControl( I2C1_BASE, I2C_MASTER_CMD_SINGLE_SEND);
  while(I2CMasterBusBusy(I2C1_BASE));

}

uint32_t getTemperature() {

  //Read the most significant byte
  I2CMasterSlaveAddrSet(I2C1_BASE, SLAVE_ADDRESS, true );
  I2CMasterControl( I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
  while(I2CMasterBusBusy(I2C1_BASE)); //Loop until the bus is no longer busy
  first_byte = I2CMasterDataGet(I2C1_BASE);
  I2CSlaveACKValueSet(SLAVE_ADDRESS, true); //Acknoledge the transfer

  I2CMasterSlaveAddrSet(I2C1_BASE, SLAVE_ADDRESS, true );
  I2CMasterControl( I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE); //Start the sequence
  while(I2CMasterBusBusy(I2C1_BASE)); //Loop until the bus is no longer busy
  second_byte = I2CMasterDataGet(I2C1_BASE);
  I2CSlaveACKValueSet(SLAVE_ADDRESS, true); //Acknoledge the transfer

  //We only care about the first 3 nibbles
  second_byte = (second_byte & 0x00F0) >> 4;

  temperature = first_byte;
  temperature = temperature << 4;
  temperature = temperature | second_byte;

  result = temperature;

  //Temperature conversion
  result = result * 0.0628;

  return result;
}
