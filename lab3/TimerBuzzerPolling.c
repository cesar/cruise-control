#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

uint8_t pin_data = 0x04;
void interrupt_handler(void);
void timer_interrupt(void);
void toggle(void);


int frequency=  1024;
uint32_t period = (SysCtlClockGet() / frequency)/2;

int main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    
    //Set up the general purpose I/Os
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);//1024hz = 15625

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, (GPIO_PIN_6 | GPIO_PIN_5));
    

    GPIOIntEnable(GPIO_PORTC_BASE, (GPIO_PIN_6 | GPIO_PIN_5));
    GPIOIntRegister(GPIO_PORTC_BASE, interrupt_handler);
    GPIOIntTypeSet(GPIO_PORTC_BASE, (GPIO_PIN_6 | GPIO_PIN_5), GPIO_FALLING_EDGE);
    toggle();
  
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, period);
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
  
  GPIOIntClear(GPIO_PORTC_BASE, (GPIO_PIN_6 | GPIO_PIN_5));

  if(GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6) == 0x40){
    recalculate_period(3);
  }
  if(GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5) == 0x40){
    recalculate_period(-3);
  }
  
    //TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, period);
    //TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
    //TimerEnable(TIMER0_BASE, TIMER_A);

}

void recalculate_period(int offset)
{
   frequency += offset;
   period = ((SysCtlClockGet() / frequency)/2);
}

void toggle(void){
 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
    SysCtlDelay(130000);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);

}

