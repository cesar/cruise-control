#include <stdint.h>

#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "inc/hw_types.h"

#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"

#include "driverlib/i2c.h"

uint8_t ui8PinData=0x08; //Note: To enable X pin, data to be sent needs to be 2^x in hexadecimal.


uint8_t pin_0 = 0x1, pin_1 = 0x2,pin_2 = 0x4, pin_3 = 0x8, pin_4 = 0x10, pin_5 = 0x20, pin_6 = 0x40, pin_7 = 0x80;

uint8_t SLAVE_ADDRESS = 0x4A, POINTER_REGISTER = 0x00; //Address of the SDA pin on the digital thermometer and the pointer address to the temperature register.

uint8_t first_byte, second_byte;



void i2c_setup(void) {

  //Enable the I2C Module
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

  //Enable the pins in port A
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  //Set PA6 as SCL
  GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);

  //Set PA7 as SDA
  GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

  //Configure SDA and SCL
  GPIOPinConfigure(GPIO_PA6_I2C1SCL);
  GPIOPinConfigure(GPIO_PA7_I2C1SDA);

  //Configure Master
  I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);

  //Enable Master
  I2CMasterEnable(I2C1_BASE);


}

uint8_t i2c_read() {

  //Set the slave address and set to write
  I2CMasterSlaveAddrSet(I2C1_BASE, SLAVE_ADDRESS, false);

  //Send the pointer address to the TMP102
  I2CMasterDataPut(I2C1_BASE, POINTER_REGISTER); //Place the data
  I2CMasterControl( I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START ); //Start the sequence
  while(I2CMasterBusBusy(I2C1_BASE)); //Loop until the bus is no longer busy

  //Read the most significant byte
  I2CMasterSlaveAddrSet(I2C1_BASE, SLAVE_ADDRESS, true );
  I2CMasterControl( I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE );
  while(I2CMasterBusBusy(I2C1_BASE)); //Loop until the bus is no longer busy

  first_byte = I2CMasterDataGet(I2C1_BASE);

  //Read the second byte of data
  I2CMasterSlaveAddrSet(I2C1_BASE, SLAVE_ADDRESS, true );
  I2CMasterControl( I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE );
  while(I2CMasterBusBusy(I2C1_BASE));

  second_byte = I2CMasterDataGet(I2C1_BASE);

  return second_byte;

}


void setup()
{
	Serial.begin(9600);
	i2c_setup();
}

void loop()
{

	int test = i2c_read();
	Serial.println(test);
	delay(1000);

}
