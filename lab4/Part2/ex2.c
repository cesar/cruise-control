#include <stdint.h>

#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "inc/hw_types.h"

#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"

uint8_t ui8PinData=0x08; //Note: To enable X pin, data to be sent needs to be 2^x in hexadecimal.


uint8_t pin_0 = 0x1, pin_1 = 0x2,pin_2 = 0x4, pin_3 = 0x8, pin_4 = 0x10, pin_5 = 0x20, pin_6 = 0x40, pin_7 = 0x80;

void sevenSegWrite();
void writeToPins();

int main(void) {
  SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
  //Enable the peripherals
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  
  //Set the input type
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2);
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_6);  
  
  int offset[] = {0,1,1,1,1,1,1,1,1,1, 8, 1, 1, 1, 1, 1, 1};
  int i = 0;
  char letter = '0';

  while(1) {
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0x00);
    sevenSegWrite(letter);
    SysCtlDelay(13333333);
    letter = letter + offset[i];
    if (i == 16) {
      i = i % 16;
      letter = '0';
    }
    else {
      i ++;
    }
  }
}


void sevenSegWrite(char letter) {

  int char_index = (letter - 'A') + 65;

  //If the number is between 0 - 9
  if(char_index < 58 && char_index > 47) {
    char_index = char_index - 48;
  }

  //If the number is between A - F
  else if(char_index > 64 && char_index < 71) {
    char_index = char_index - 55;
  }
  
  int lookUpTable[16][7] = {
    {0,1,1,1,1,1,1}, //0
    {0,0,0,0,1,1,0}, //1
    {1,0,1,1,0,1,1}, //2
    {1,0,0,1,1,1,1}, //3
    {1,1,0,0,1,1,0}, //4
    {1,1,0,1,1,0,1}, //5
    {1,1,1,1,1,0,1}, //6
    {0,0,0,0,1,1,1}, //7
    {1,1,1,1,1,1,1}, //8
    {1,1,0,0,1,1,1}, //9
    {1,1,1,0,1,1,1}, //A - 10
    {1,1,1,1,1,1,1}, //B - 11
    {0,1,1,1,0,0,1}, //C - 12
    {0,1,1,1,1,1,1}, //D - 13
    {1,1,1,1,0,0,1}, //E - 14
    {1,1,1,0,0,0,1}  //F - 15
  };
  writeToPins(lookUpTable[char_index]);
}

void writeToPins(int *numbers) {

  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, pin_2 * numbers[2] + pin_3 *numbers[3]);
  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, pin_6 * numbers[5]);
  GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7) , pin_4 * numbers[4] + pin_5 * numbers[0] + pin_6 * numbers[1] + pin_7 * numbers[6]);
}


