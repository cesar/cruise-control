/* Include header files */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

/* Function prototypes */

uint32_t b1 = UART1_BASE;
uint32_t b5 = UART5_BASE;

/* Program flow prototypes */

void toggleLED();
void setupLCD();

/* LCD prototypes */
void putChar(char character);
void clearDisplay();
void putPhrase(char *string);
void selectLineOne();
void selectLineTwo();
void changeCursorUnderscore();

/* Main body and loop */
int main(void){

	//Setup functions
	toggleLED();
	setupLCD();

	//Main loop
	while(1){

	}

}


/* Functions Declarations */

/* Program flow functions */

void toggleLED(){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
	SysCtlDelay(3000000);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);	
	SysCtlDelay(3000000);
}

void setupLCD(){
	//Enable ports and initialize pins
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_1);
	//Initialize UART, 9600 baud, 1 stop bit, and no parity bit
	UARTConfigSetExpClk(b1, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTEnable(b1);
	//Change cursor
	changeCursorUnderscore();
	clearDisplay();		



}

/* LCD functions */

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