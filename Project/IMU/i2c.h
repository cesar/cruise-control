#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C"
{
#endif



void setup_i2c();
uint8_t i2c_read(uint8_t address);
int i2c_write(uint8_t address, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_ADC_H__