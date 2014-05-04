#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"

uint8_t GYRO_SLAVE_ADDRESS = 0x68, ACC_SLAVE_ADDRESS = 0x53;

uint8_t byte_data;

uint32_t gyroX, gyroY, gyroZ, x, y, z;

void i2c_setup(void) {

  //Enable the I2C Module
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

  //Wait at least 5 clock cycles
  SysCtlDelay(2);

  //Configure SDA and SCL
  GPIOPinConfigure(GPIO_PE4_I2C2SCL);

  GPIOPinConfigure(GPIO_PE5_I2C2SDA);

  //Wait at least 5 clock cycles
  SysCtlDelay(2);

    //Set PE4 as SCL
  GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);

  //Set PE5 as SDA
  GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);

  //Configure Master,
  I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false);

}

/*
* uint8_t address - address of the slave device
* uint8_t data - data to be written
* function - write to the slave device
* returns - 1 for successful write, 0 for failure
*/
int i2c_write(uint8_t address, uint8_t data) {

  I2CMasterSlaveAddrSet(I2C2_BASE, address, false);

  I2CMasterDataPut(I2C2_BASE, data);

  I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

  while(I2CMasterBusBusy(I2C2_BASE)); //Loop until the bus is no longer busy
  
  //Delay
  SysCtlDelay(5000);
  
  if(I2CMasterErr(I2C2_BASE) == I2C_MASTER_ERR_NONE) {
    //there was no error
    return 1;
  } else {
    return 0;
  }
}

/*
* uint8_t address - slave address from which to read
* function - read data from the slave device
* returns - a byte of data read from the slave device
*/
uint8_t i2c_read(uint8_t address) {
  
  I2CMasterSlaveAddrSet(I2C2_BASE, address, true );

  I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

  while(I2CMasterBusBusy(I2C2_BASE)){}; //Loop until the bus is no longer busy
  
  //Delay
  SysCtlDelay(5000);

  if(I2CMasterErr(I2C2_BASE) == I2C_MASTER_ERR_NONE) {
    byte_data = I2CMasterDataGet(I2C2_BASE);
    return byte_data;
  } else {
    return 0x00;
  } 
}

//Update the register values, constantly keep the latest values in the 
void update() {
  //Reset the preset values
  gyroX = 0x00;
  gyroY = 0x00;
  gyroZ = 0x00;

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

  x = gyroX;

  y = gyroY;

  z = gyroZ;
}

float getX() {
  return (float)x / 14.375 ;
}

float getY() {
  return (float) y / 14.375;
}

float getZ() {
  return (float) z / 14.375 ; 
}
//FOR LE TESTING***********************

void setup()
{
  Serial.begin(9600);
  i2c_setup();
}

void loop()
{
  update();

  Serial.print("X : ");
  Serial.print(getX());
  Serial.print(" | ");  
  Serial.print("Y: ");
  Serial.print(getY());
  Serial.print(" | ");
  Serial.print("Z: ");
  Serial.println(getZ());
  
  delay(1000);
}

