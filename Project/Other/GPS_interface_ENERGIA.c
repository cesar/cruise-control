#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

void hotStartReset();
void enableFiveHertzCommunication();
void enableVTG();
void enableBaud9600();
void enableBaud4800();
void enableSBAS();
void disableSBAS();
void waitForData();
void coldStartReset();
void setLCDBrightness40();
void disableRMC();
//Program flow commands
void toggleLED();

uint32_t b5 = UART5_BASE;

char hot_start_reset[12] = {0XB5, 0X62, 0X06, 0X04, 0X04, 
	0X00, 0X00, 0X00, 0X02, 0X00, 0X10, 0X68};
char cold_start_reset[12] = {0XB5, 0X62, 0X06, 0X04, 0X04, 
	0X00, 0XFF, 0X87, 0X02, 0X00, 0X96, 0XF9};

char five_hertz[14] = {0XB5, 0X62, 0X06, 0X08, 0X06, 0X00, 0XC8, 
	0X00, 0X01, 0X00, 0X10, 0X00, 0xDE, 0x6A};
char vtg_data[16] = {0XB5, 0X62, 0X06, 0X01, 0X08, 0X00, 0XF0, 
	0X05, 0X00, 0X01, 0X00, 0X00, 0X00, 0X01, 0X06, 0X4C};
char rmc_data_disable[16] = {0XB5, 0X62, 0X06, 0X01, 0X08, 0X00, 
	0XF0, 0X04, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X04, 0X40};
char baudRate9600[28] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00,
 	0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x07, 0x00,
 	0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA6, 0xCD};
char baudRate4800[28] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 
	0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0xC0, 0x12, 0x00, 0x00, 0x07, 0x00, 
	0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD3, 0xFC};
char enableSBASCommand[16] = {0xB5, 0x62, 0x06, 0x10, 0x08, 0x00, 0x01, 0x03, 
	0x03, 0x00, 0x51, 0x62, 0x06, 0x00, 0xE3, 0x27};
char disableSBASCommand[16] = {0xB5, 0x62, 0x06, 0x10, 0x08, 0x00, 0x01, 0x03,
 0x03, 0x00, 0x51, 0x62, 0x06, 0x00, 0xE4, 0x2F};
char data_string[100];
char UTCTime[10];
char char_check;

void setup(){
	//GPIO enable
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	//UART enable
	//GPS UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);

	//GPIO pin type configure
	//From and to GPS
	GPIOPinConfigure(GPIO_PE4_U5RX);
	GPIOPinConfigure(GPIO_PE5_U5TX);

	//Red led
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	//RX, TX to GPS
	GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	//Initialize the UART.
	//Baud rate - Data bits - turn off parity - stop bits --- 9600 - 8 - Off - 1
	UARTConfigSetExpClk(b5, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

	UARTEnable(b5);
	toggleLED();

	//---------- GPS settings ----------//
	//enableFiveHertzCommunication();
	//enableVTG();
	//enableSBAS();
	//coldStartReset();
	//hotStartReset();
	//disableRMC();
	//---------- GPS settings ----------//

	Serial.begin(9600);
	toggleLED();
}

void loop(){
	char_check = (char) UARTCharGet(b5);
	if(char_check == '$') Serial.print('\n');
	Serial.print(char_check);
}


void toggleLED(){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
	SysCtlDelay(3000000);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);	
	SysCtlDelay(3000000);

}
void enableFiveHertzCommunication(){
	for(int i = 0; i < 12; i++){
		UARTCharPut(b5, five_hertz[i]);
	}
}

void hotStartReset(){
	for(int i = 0; i < 12; i++){
		UARTCharPut(b5, hot_start_reset[i]);
	}
}

void enableVTG(){
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, vtg_data[i]);
	}
}

void enableBaud9600(){ 
	for( int i = 0; i < 28; i++){
		UARTCharPut(b5, baudRate9600[i]);
	}
}

void enableBaud4800(){
	for( int i = 0; i < 28; i++){
		UARTCharPut(b5, baudRate4800[i]);
	}
}

void enableSBAS(){
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, enableSBASCommand[i]);
	}
}
void disableSBAS(){
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, disableSBASCommand[i]);
	}
}

void waitForData(){
	while(!UARTCharsAvail(b5)){}
}

void coldStartReset(){
	for(int i = 0; i < 12; i++){
		UARTCharPut(b5, cold_start_reset[i]);
	}
}

void disableRMC(){
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, rmc_data_disable[i]);
	}
}