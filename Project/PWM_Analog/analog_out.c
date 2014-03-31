#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
// int main(void) {
//     uint32_t ulPeriod, dutyCycle;

//     // 40 MHz system clock
//     SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|
//         SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

//     ulPeriod = 1000;
//     dutyCycle = 500;

  
//     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//     GPIOPinConfigure(GPIO_PB6_T0CCP0);
//     GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);

//     // Configure timer
//     SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
//     TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
//     TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod -1);
//     TimerMatchSet(TIMER0_BASE, TIMER_A, dutyCycle); // PWM
//     TimerEnable(TIMER0_BASE, TIMER_A);

  
// }
void setup()
{
    Serial.begin(9600);
   uint32_t ulPeriod, dutyCycle;

    // 40 MHz system clock
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|
        SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    ulPeriod = 1000;
    dutyCycle = 250;   

  
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB6_T0CCP0);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);

    // Configure timer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
    TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod -1);
    TimerMatchSet(TIMER0_BASE, TIMER_A, dutyCycle); // PWM
    TimerEnable(TIMER0_BASE, TIMER_A);

}

void loop()
{


}