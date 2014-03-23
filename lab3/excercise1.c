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

int frequency=  160000;
uint32_t period = (SysCtlClockGet() / frequency)/2;

int main(void) {

    
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    
    //Set up the general purpose I/Os
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);//1024hz = 15625

    //Set up the input/output pins
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6);
    
    //Set up the interrupt pins
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_5 | GPIO_INT_PIN_6);
    GPIOIntRegister(GPIO_PORTC_BASE, interrupt_handler);
    GPIOIntTypeSet(GPIO_PORTC_BASE, (GPIO_PIN_5 | GPIO_PIN_6), GPIO_RISING_EDGE);

    //Just a quick toggle to make sure code is working
    toggle();
  
    //Timer Configuration
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, frequency);
    TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
    TimerEnable(TIMER0_BASE, TIMER_A);

    //Timer Interrupt Enable
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER0_BASE, TIMER_A, timer_interrupt);

    //Loop forever
    while(1){}
}

//Configure the timer interrupt handler
void timer_interrupt(void) {
    //Clear the timer interrupt pin
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, pin_data);
    pin_data^=0x04;
}

//Button interrupt handler
void interrupt_handler(void){
  
  //Clear the interrupt ports
  GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_5 | GPIO_INT_PIN_6);
  
  if(GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5) == 0x20){
    frequency -= 10000;}
  if(GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6) == 0x40){
    frequency += 10000;}
    
    //Set new timer frequency
    TimerLoadSet(TIMER0_BASE, TIMER_A, frequency);
}

void toggle(void){
 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
    SysCtlDelay(130000);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);

}

