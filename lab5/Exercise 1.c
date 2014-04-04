#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

void putChar(char character);
void clearDisplay();
void putPhrase(char *string);
void selectLineOne();
void selectLineTwo();
void changeCursorUnderscore();
void toggleLED();
int enter_flag = 0;
char letter[16];


uint32_t b1 = UART1_BASE; 
uint32_t b5 = UART5_BASE;

void setup()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
	//Configure pins due to bit banging
        //Transfer pin to lcd
	GPIOPinConfigure(GPIO_PB1_U1TX);
        //Receiving pin from usb
	GPIOPinConfigure(GPIO_PE4_U5RX);
        //Transfer pin to usb
	GPIOPinConfigure(GPIO_PE5_U5TX);
        //LED pin
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
        //lcd pin
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_1);
        //usb pins
	GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	//Initialize the UART. 
	//Baud rate - Data bits - turn off parity - stop bits --- 9600 - 8 - Off - 1
	UARTConfigSetExpClk(b1, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTEnable(b1);
	UARTConfigSetExpClk(b5, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTEnable(b5);
	toggleLED();
	changeCursorUnderscore();
	clearDisplay();		
	putPhrase("Hello World!");  
	SysCtlDelay(3000000);
	clearDisplay();		

	//Test terminal
	UARTCharPut(b5, 'h');
	UARTCharPut(b5, 'e');
	UARTCharPut(b5, 'l');
	UARTCharPut(b5, 'l');
	UARTCharPut(b5, 'o');

}

void loop(){

	char le = (char) UARTCharGet(b5);
	putChar(le);
	UARTCharPut(b5, (char)((int) le - 32));
	SysCtlDelay(3000);

}

//Place character
void putChar(char character){
	UARTCharPut(b1, character);
}

//Places many characters
void putPhrase(char *string){
	int i;
	for(i = 0; i < 16; i++){
		putChar(string[i]);
	}
}
//Clears the display
void clearDisplay(){
	UARTCharPut(b1, 0xFE);
	UARTCharPut(b1, 0x01);
}
//Toggle the red led
void toggleLED(){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
	SysCtlDelay(3000000);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);	
	SysCtlDelay(3000000);
}
//Place cursor at the first line
void selectLineOne(){
	UARTCharPut(b1, 0xFE);
	UARTCharPut(b1, 0x80);
}
//Place the cursor at the second line
void selectLineTwo(){
	UARTCharPut(b1, 0xFE);
	UARTCharPut(b1, 0xC0);
}
//Change the cursor appearance to a blinking underscore
void changeCursorUnderscore(){
	UARTCharPut(b1, 0xFE);
	UARTCharPut(b1, 0x0E);
}