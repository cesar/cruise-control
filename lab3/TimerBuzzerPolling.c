#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

uint8_t pin_data = 0x04;
void interrupt_handler(void);
void toggle(void);

int frequency = 15000;
int main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //Set up the general purpose I/Os
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);//1024hz = 15625
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
    GPIOIntRegister(GPIO_PORTF_BASE, interrupt_handler);
    GPIOIntTypeSet(GPIO_PORTF_BASE, (GPIO_PIN_0 | GPIO_PIN_4), GPIO_BOTH_EDGES);
    toggle();
  
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, frequency);
    TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
    TimerEnable(TIMER0_BASE, TIMER_A);
    while(1){
          
      if(TimerValueGet(TIMER0_BASE, TIMER_A) == 0){
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, pin_data);
                pin_data^=0x04;
          }
        }



}

void interrupt_handler(void){
  
  GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
  if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0x01){
    frequency -= 1000;}
  else if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x10){
    frequency += 1000;}
  
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, frequency);
    TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
    TimerEnable(TIMER0_BASE, TIMER_A);

}

void toggle(void){
 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
    SysCtlDelay(130000);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);

}

