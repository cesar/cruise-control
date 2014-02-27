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
        
        //Set LED for output
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
        
        //Set button for input
        GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);    
        
        //Enable pin for interrupt
  GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);

        //Set ISR
  GPIOIntRegister(GPIO_PORTF_BASE, interrupt_handler);
        
        //Enable pin after interrupt being serviced
  GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);

        while(1){
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, pin_data);
        SysCtlDelay(130000);

        };
        
}

void interrupt_handler(void)
{


  GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);

  pin_data = pin_data ^ 0x2;


}