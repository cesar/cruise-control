/* Remove any NMEA sentences that do not provide speed readings and print out an average value */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
//GPS control commands
void coldStartReset();
void hotStartReset();
void enableSBAS();
void disableSBAS();
void disableRMC();
void disableGGA();
void disableGLL();
void disableGSA();
void disableGSV();
void disableVTG();
//Program flow commands
void toggleLED();
void printTime();
void getTime();

void getVelocity(char *rmcSentence);
void printVelocity(char *velocityReading);

void genericReader();

uint32_t b5 = UART5_BASE;

char data_string[100];
char UTCTime[7] = "000000";
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
	//coldStartReset();
	hotStartReset();
	enableSBAS();
	disableGGA();
	disableGLL();
	disableGSV();
	disableGSA();
	//disableRMC();
	//---------- GPS settings ----------//

	Serial.begin(9600);
	toggleLED();
}

void loop(){
	
	getTime();
	//genericReader();
	
}

void getTime(){
	char temp;
	//Wait for a new sentence
	while((char) UARTCharGet(b5) != '$'){}
	//Pass the information to the string
	for(int i = 0; temp != '$' && i < 100; i++){
		temp = (char) UARTCharGet(b5);
		data_string[i] = temp;
		// Serial.print(i);
		// Serial.print(' ');
		// Serial.print(temp);
		// Serial.print('\n');
	}
	//If the string contains RMC data, place the time into UTCTime
	//$GPVTG,34.64,T,,M,10.744,N,19.898,K,A*0F

	if((data_string[2] == 'R') && (data_string[3] == 'M') && (data_string[4] == 'C')){
		if(data_string[6] == ','){
			Serial.print("Time: 99:99:99\n");
			return;
		}
		else{
			UTCTime[0] = data_string[6];
			UTCTime[1] = data_string[7];
			UTCTime[2] = data_string[8];
			UTCTime[3] = data_string[9];
			UTCTime[4] = data_string[10];
			UTCTime[5] = data_string[11];
			printTime();
		}
		//Serial.print(data_string);

	}
	else if((data_string[2] == 'V') && (data_string[3] == 'T') && (data_string[4] == 'G')){
		getVelocity(data_string);
	}
}

void printTime(){
	for(int i = 0; i < 6; i++){
		if(i == 2 || i == 4){
			Serial.print(':');
		}
		Serial.print(UTCTime[i]);
	}
	Serial.print('\n');
}

void getVelocity(char *rmcSentence){
	//We have the RMC sentence that contains speed after the seventh comma
	int commaCounter = 0;
	char velocityReading[6];
	int speedIndex;
	//Count the index at the speed reading
	for(speedIndex = 0; commaCounter < 7; speedIndex++){
		//If there is a comma, count until 7
		if(rmcSentence[speedIndex] == ','){
			commaCounter++;
		}
	}
	//Current speed index is at the comma, we add one to check the next numeric value
	speedIndex++;

	//If the character at speedIndex is a comma, there is no data available
	if(rmcSentence[speedIndex] == ','){
		for(int i = 0; i < 6; i++){
			velocityReading[i] = '0';
		}
	}
	//Else, we write the speed value to the velocityReading string
	else{
		for(int k = 0; rmcSentence[speedIndex + k] != ','; k++){
			velocityReading[k] = rmcSentence[speedIndex + k];
		}
	}
	printVelocity(velocityReading);
}

void printVelocity(char *velocityReading){
	//int speedInteger = atoi(velocityReading);//integer in knots
	for(int i = 0; i < 4; i++){
		Serial.print(velocityReading[i]);
	}
	Serial.print(" kmh.\n");

	// float speedMPH = 1.15 * speedInteger;
	// Serial.print(speedMPH);
	// Serial.print(" MPH\n");

}

void toggleLED(){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
	SysCtlDelay(3000000);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);	
	SysCtlDelay(3000000);

}

void genericReader(){
	char_check = (char) UARTCharGet(b5);
	if(char_check == '$') Serial.print('\n');
	Serial.print(char_check);
}




/****************************************************************************************************************************/
//SBAS enable and disable
void enableSBAS(){
	char enableSBASCommand[16] = {0xB5, 0x62, 0x06, 0x10, 0x08, 0x00, 0x01, 0x03, 0x03, 0x00, 0x51, 0x62, 0x06, 0x00, 0xE3, 0x27};
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, enableSBASCommand[i]);
	}
}
void disableSBAS(){
	char disableSBASCommand[16] = {0xB5, 0x62, 0x06, 0x10, 0x08, 0x00, 0x01, 0x03, 0x03, 0x00, 0x51, 0x62, 0x06, 0x00, 0xE4, 0x2F};
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, disableSBASCommand[i]);
	}
}

//Cold and hot reset, takes into account previous session's ephemeris
void coldStartReset(){	
	char cold_start_reset[12] = {0XB5, 0X62, 0X06, 0X04, 0X04, 0X00, 0XFF, 0X87, 0X02, 0X00, 0X96, 0XF9};
	for(int i = 0; i < 12; i++){
		UARTCharPut(b5, cold_start_reset[i]);
	}
}
void hotStartReset(){
	char hot_start_reset[12] = {0XB5, 0X62, 0X06, 0X04, 0X04, 0X00, 0X00, 0X00, 0X02, 0X00, 0X10, 0X68};
	for(int i = 0; i < 12; i++){
		UARTCharPut(b5, hot_start_reset[i]);
	}
}

//Disables RMC, we need RMC to get time and velocity and is enabled defaultly
void disableRMC(){
	char rmc_data_disable[16] = {0XB5, 0X62, 0X06, 0X01, 0X08, 0X00, 0XF0, 0X04, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X04, 0X40};
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, rmc_data_disable[i]);
	}
}

//Disable unneeded NMEA sentences
void disableGGA(){
	char gga_data_disable[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x24};
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, gga_data_disable[i]);
	}


}
void disableGLL(){
	char gll_data_disable[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2b};
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, gll_data_disable[i]);
	}

}
void disableGSA(){
	char gsa_data_disable[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x32};
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, gsa_data_disable[i]);
	}


}
void disableGSV(){
	char gsv_data_disable[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39};
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, gsv_data_disable[i]);
	}
}
void disableVTG(){
	char vtg_data_disable[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x47};
	for(int i = 0; i < 16; i++){
		UARTCharPut(b5, vtg_data_disable[i]);
	}
}

