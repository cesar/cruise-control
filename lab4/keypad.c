#include <stdint.h>

#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "inc/hw_types.h"

#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"

#include "driverlib/uart.h"

#include "driverlib/timer.h"
void putChar(char character);
void clearDisplay();
void putPhrase(char *string);
void selectLineOne();
void selectLineTwo();
void changeCursorUnderscore();
void toggleLED();
char ser_lcd_command;
uint32_t b1 = UART1_BASE;

//Using PB3 as the Output Pin

//User for scannind and reading
int log_scan = 0x1;
int scan_code = 0x1;
char lookup_table[] = {'x','1','2','3','x','4', '5', '6','x', '7','8','9','x', '*','0', '#'};
char key_char = 'x';


uint8_t ui8PinData=0; //Note: To enable X pin, data to be sent needs to be 2^x in hexadecimal.
int isr_flag = 0;
uint32_t system_delay = 500000;
int frequency;
int frequency_b;

uint8_t pin_0 = 0x1, pin_1 = 0x2,pin_2 = 0x4, pin_3 = 0x8, pin_4 = 0x10, pin_5 = 0x20, pin_6 = 0x40, pin_7 = 0x80;

int main(void) {
  SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);



  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);//1024hz = 15625
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);//1024hz = 15625




  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
  GPIOPinConfigure(GPIO_PB1_U1TX);
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);


  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_1));
  GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  frequency_b =(SysCtlClockGet()/1)/2;
  frequency = (SysCtlClockGet()/100)/2;

  changeCursorUnderscore();

  //Clear Display
  clearDisplay();
  putPhrase("Hello World!");
  //Timer Configuration
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);

  TimerLoadSet(TIMER0_BASE, TIMER_A, frequency);
  TimerLoadSet(TIMER2_BASE, TIMER_A, frequency_b);

  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  TimerControlEvent(TIMER2_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);

  TimerEnable(TIMER0_BASE, TIMER_A);
  TimerEnable(TIMER2_BASE, TIMER_A);


  //Timer Interrupt Enable
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);


  TimerIntRegister(TIMER0_BASE, TIMER_A, timer_interrupt);
  TimerIntRegister(TIMER2_BASE, TIMER_A, timer1_interrupt);


  while(1){

  }

}

void timer_interrupt(void)
{
  TimerIntClear(TIMER0_BASE, TIMER_A);
  //Send SCan Code to scan port
  GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_1), scan_code);


  //Get return code form return port
  //CHECK THIS PART
  int code = GPIOPinRead(GPIO_PORTA_BASE, (GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7));
  int return_code = 0;

  if(code == pin_5) return_code = 0x1;
  if(code == pin_6) return_code = 0x2;
  if(code == pin_7) return_code = 0x3;

  //


  int index = (log_scan << 2) + return_code;

  key_char = lookup_table[index];

  scan_code = scan_code << 1;
  // rotate left scancode

  // increment logscan by  1
  log_scan++;

  if(log_scan > 3)
  {
    scan_code =0x1;
    log_scan = 0;
  }



}
void timer1_interrupt(void)
{
  TimerIntClear(TIMER2_BASE, TIMER_A);
  //Check if a character is chosen
  //toggleLed() or something



}


void toggleLED(){
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
  SysCtlDelay(3000000);
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
  SysCtlDelay(3000000);

}
