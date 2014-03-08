#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

uint8_t pin_data = 0x04;  
void toggle(void);

int main(void) { 

//SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
//Set up the general purpose I/Os
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); 
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);//1024hz = 15625
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_1); 
    toggle();


//Set up the timer
// 	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
//     toggle();
// 	TimerLoadSet(TIMER0_BASE, TIMER_A, 15625);
//    toggle();
// 	TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
//    toggle();
//	TimerEnable(TIMER0_BASE, TIMER_A);
//    toggle();
//	
        
        while(1){
	  //if(TimerValueGet(TIMER0_BASE, TIMER_A) == 0){
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, pin_data);
                SysCtlDelay(8000);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);
                SysCtlDelay(8000);
                
          //}
        }



}

void toggle(void){
 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
    SysCtlDelay(130000);    
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);

}

