#include <stdint.h> 

#include <stdbool.h> 

#include "inc/hw_memmap.h" 

#include "inc/hw_types.h" 

#include "driverlib/sysctl.h" 

#include "driverlib/gpio.h" 

 
//Using PB3 as the Output Pin


uint8_t ui8PinData=0x08; //Note: To enable X pin, data to be sent needs to be 2^x in hexadecimal.

 

int main(void) { 
 
 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);  

 GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3); 

 while(1) { 
  
   GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, ui8PinData); 
 
   ui8PinData = ui8PinData ^ 0x08; //

   SysCtlDelay(3000000); 
  }
}
