#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
void toggleLED();
char lookup_table[] = {'x','1','2','3','x','4', '5', '6','x', '7','8','9','x', '*','0', '#'};
char key_char = 'x';
int log_code = 0, code = 0, index_code = 0, counter = 0, temp;
int main(void){

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);

	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	toggleLED();
	// GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0X8);
	// GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x10);
	// GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0x20);
	// GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x40);


	while(1){
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

		if(key_char == '1'){
			toggleLED();
			//SysCtlDelay(100000);
			index_code = 0;
		}


		counter++;
		if(counter > 3) counter = 0;
	}
}

void toggleLED(){
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
  SysCtlDelay(3000000);
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
}