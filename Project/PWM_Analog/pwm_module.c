#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"

void analog_out_setup()
{
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB6_T0CCP0);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);

    //Testing purposes
    analog_out(1000, 250);

}
void analog_out(uint32_t ui32Period, uint32_t duty_cycle){

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
    TimerMatchSet(TIMER0_BASE, TIMER_A, duty_cycle); // PWM
    TimerEnable(TIMER0_BASE, TIMER_A);

}
//Testing purposes
int main(void){

    analog_out_setup();
}