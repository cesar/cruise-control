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

  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

  //Set ISR
  GPIOIntRegister(GPIO_PORTF_BASE, interrupt_handler);
  //Enable pin for interrupt
  GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
  //Set interrupt type
  GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_RISING_EDGE);

  
    while(1){}
  return 0;
}

void interrupt_handler(){
  GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
  
  if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x10){
    pin_data^=0x02;
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, pin_data);
    SysCtlDelay(300000);
    //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
    //SysCtlDelay(300000);
    //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, pin_data);
    //SysCtlDelay(300000);
  }
  //GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
}