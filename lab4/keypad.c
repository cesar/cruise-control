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

uint32_t b1 = UART1_BASE;
char lookup_table[] = {'x','1','2','3','x','4', '5', '6','x', '7','8','9','x', '*','0', '#'};
char key_char = 'x';
int log_code = 0, code = 0, index_code = 0, counter = 0, temp, flag = 0;

//Using PB1 as the Output Pin

//User for scannind and reading


uint8_t ui8PinData=0; //Note: To enable X pin, data to be sent needs to be 2^x in hexadecimal.
int isr_flag = 0;
uint32_t system_delay = 500000;
int frequency;

uint8_t pin_0 = 0x1, pin_1 = 0x2,pin_2 = 0x4, pin_3 = 0x8, pin_4 = 0x10, pin_5 = 0x20, pin_6 = 0x40, pin_7 = 0x80;

int main(void) {
  SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);//1024hz = 15625
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
  GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);
  GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIOPinConfigure(GPIO_PB1_U1TX);
  GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_1);
  UARTConfigSetExpClk(b1, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
  UARTEnable(b1);

  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);


  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, (GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_1));
  GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  frequency = SysCtlClockGet()/2;

  changeCursorUnderscore();
  toggleLED();


  //Clear Display
  clearDisplay();
  //putPhrase("Hello World!");
  //Timer Configuration
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerLoadSet(TIMER0_BASE, TIMER_A, frequency);
  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  TimerEnable(TIMER0_BASE, TIMER_A);

  //Timer Interrupt Enable
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  TimerIntRegister(TIMER0_BASE, TIMER_A, timer_interrupt);


  while(1){
    if(flag = 1){  
    //Reset the line read
      code = 0;

    //Turn on the scan for a line
      if(counter == 0) GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0x8);
      if(counter == 1) GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x10);
      if(counter == 2) GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x20);
      if(counter == 3) GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x40);

    //Check which button on the line was pressed
      if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5) == 0x20) {
        code = 1;
        temp = log_code;
      }
      if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6) == 0x40) {
        code = 2;
        temp = log_code;
      }
      if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7) == 0x80) {
        code = 3;
        temp = log_code;
      }

    //Turn off the scan for a line
      if(counter == 0) GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0x0);
      if(counter == 1) GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x0);
      if(counter == 2) GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x0);
      if(counter == 3) GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x0);

    //Check the next line on the next run and if overflows, reset


    //Calculate the index_code for the lookup table
      index_code = temp + code;
      key_char = lookup_table[index_code];
      log_code = log_code + 4;
      if(log_code > 12){
        log_code = 0;
      }

      counter++;
      if(counter > 3) counter = 0;
      if(key_char != 'x'){
        //toggleLED();
        putChar(key_char);
      //SysCtlDelay(100000);
        index_code = 0;
      }
      flag = 0;
    }
  }

}

void timer_interrupt(void)
{
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  flag = 1;
  


}

void toggleLED(){
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
  SysCtlDelay(3000000);
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
  SysCtlDelay(3000000);
}


void putChar(char character){
  //Place character
  UARTCharPut(b1, character);
}

void putPhrase(char *string){
  int i;
  for(i = 0; i < 12; i++){
    putChar(string[i]);
  }
}

void clearDisplay(){
  UARTCharPut(b1, 0xFE);
  UARTCharPut(b1, 0x01);
}

void selectLineOne(){
  UARTCharPut(b1, 0xFE);
  UARTCharPut(b1, 0x80);
}

void selectLineTwo(){
  UARTCharPut(b1, 0xFE);
  UARTCharPut(b1, 0xC0);
}

void changeCursorUnderscore(){
  UARTCharPut(b1, 0xFE);
  UARTCharPut(b1, 0x0E);
}