#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#define GPIO_PB6_T0CCP0         0x00011807
#define GPIO_PIN_6              0x00000040  // GPIO pin 6

void setup_pwm()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB6_T0CCP0);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
}

void pwm_out(uint32_t ui32Period, uint32_t duty_cycle)
{
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
    TimerMatchSet(TIMER0_BASE, TIMER_A, duty_cycle); // PWM
    TimerEnable(TIMER0_BASE, TIMER_A);

}


