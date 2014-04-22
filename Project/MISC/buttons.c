#include <stdint.h>

#include <stdbool.h>

#include <math.h>

#include "inc/hw_memmap.h"

#include "inc/hw_types.h"

#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"

#include "driverlib/i2c.h"

//Flags
int velocity, enableSys;

void bHandler(void);

void setupButtons() {

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4);

  GPIOIntEnable(GPIO_PORTF_BASE, (GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_4));

  GPIOIntRegister(GPIO_PORTF_BASE, bHandler);

  GPIOIntTypeSet(GPIO_PORTF_BASE, (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_INT_PIN_4) , GPIO_RISING_EDGE);

}

void bHandler(void) {
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_4);

    //Increase velocity
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) == GPIO_PIN_1) {

    }
    //Decrease velocity
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) == GPIO_PIN_2) {

    }
    //Enable Disable
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == GPIO_PIN_4) {
      //Toggle the flag, add a timed delay

      if(enableSys == 1) {
        enableSys = 0;
      }
      else {
        enableSys = 1;
      }
    }
}