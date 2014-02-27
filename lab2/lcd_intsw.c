#include <stdint.h> 

#include <stdbool.h> 

#include <string.h>

#include "inc/hw_memmap.h" 

#include "inc/hw_types.h" 

#include "driverlib/sysctl.h" 

#include "driverlib/gpio.h" 

uint8_t pin_data = 0x2;

void interrupt_handler(void);


int main(){


	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);

	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);


	GPIOIntRegister(GPIO_PORTF_BASE, interrupt_handler);

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, pin_data);

}

void interrupt_handler(void)
{


	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);

	pin_data = pin_data ^ 0x2;

	GPIOIntRegister(GPIO_PORTF_BASE, interrupt_handler);

}