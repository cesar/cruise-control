#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
// #include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
void setup_peripherals();
void putChar(char character);
void clearDisplay();
void putPhrase(char *string);
char ser_lcd_command;
uint32_t b1 = UART1_BASE; 
int main(void){
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
	setup_peripherals();
	//Initialize the UART. UART is base 7 in PE1.
	//Baud rate - Data bits - turn off parity - stop bits --- 9600 - 8 - Off - 1
	UARTConfigSetExpClk(b1, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTEnable(b1);
	
	putPhrase("Hello World!");
	while(1){
		if (UARTCharsAvail(UART1_BASE))
		{
			 UARTCharPut(UART1_BASE, UARTCharGet(UART1_BASE));
		}		
	}
}
void putPhrase(char *string){
	int i;
	for(i = 0; i < 12; i++){
		putChar(string[i]);
	}
}

void putChar(char character){
	//Place character
	UARTCharPut(b1, character);
}
void setup_peripherals()
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_1);
}

