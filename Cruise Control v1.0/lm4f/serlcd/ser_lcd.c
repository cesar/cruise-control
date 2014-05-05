#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "ser_lcd.h"
#define GPIO_PB1_U1TX           0x00010401


uint32_t b1 = UART1_BASE; //UART1

//Setup LCD, Peripheral Enable
void setup_LCD(){

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_1);

}

//Enable LCD UART Protocol
void enable_LCD()
{
	//Baud rate = 9600, uart1 enabled, base clock chosen and uart configuration at a buffer size of 8
	UARTConfigSetExpClk(b1, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTEnable(b1);
	changeCursorUnderscore();
	clearDisplay();

}

//Put char in the UART buffer
void putChar(char character){
	//Place character
	UARTCharPut(b1, character);
}

//Put a phrase in the UART Buffer
void putPhrase(char *string){
	int i;
	for(i = 0; i < strlen(string); i++){
		putChar(string[i]);
	}
}

//CLear the LCD Display
void clearDisplay(){
	UARTCharPut(b1, 0xFE);
	UARTCharPut(b1, 0x01);
}

//Select the first line of the LCD
void selectLineOne(){
	UARTCharPut(b1, 0xFE);
	UARTCharPut(b1, 0x80);
}

//Select the second line of the LCD
void selectLineTwo(){
	UARTCharPut(b1, 0xFE);
	UARTCharPut(b1, 0xC0);
}

//Refresh the underscore of the LCD
void changeCursorUnderscore(){
	UARTCharPut(b1, 0xFE);
	UARTCharPut(b1, 0x0E);
}