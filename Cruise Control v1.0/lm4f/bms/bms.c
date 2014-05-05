#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "bms.h"

#define GPIO_PC6_U3RX 0x00021801
#define GPIO_PC7_U3TX 0x00021C01

char data_string[100];
char load[2];

void setupBMS(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
	GPIOPinConfigure(GPIO_PC6_U3RX);
	GPIOPinConfigure(GPIO_PC7_U3TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);//change to 57600 when using the actual bms

}

void sendStartSequence(){
	UARTCharPut(UART3_BASE, '2');
	UARTCharPut(UART3_BASE, '6');
}

void sendReset(){
	UARTCharPut(UART3_BASE, 'H');
}

char *getDataString(){
	int i;
	char temp = '0';
	for(i = 0; temp != 0x03 ; i++){
		temp = UARTCharGet(UART3_BASE);
		data_string[i] = temp;
	}

	return data_string;

}

char *getLoad(){
	int i, index = 0;
	char temp = '0';
	for(i = 0; i < 20; index++){
		temp = data_string[index];
		if(temp == 0x20) i++;
	}

	load[0] = data_string[index+1];
	load[1] = data_string[index+2];
	//load[2] = data_string[index+3];

	return load;

}