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
#include "gps635.h"
//GPS control commands


#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE5_U5TX           0x00041401


uint32_t b5 = UART5_BASE;

char data_string[100];
char UTCTime[8] = "00:00:01";
char char_check;
char velocityReading[6];
char speed[6];
char latitude[11];//ddmm.mmmmX
char longitude[12];//ddmm.mmmmX

void setup_GPS()
{	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	//UART enable
	//GPS UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);

	//GPIO pin type configure
	//From and to GPS
	GPIOPinConfigure(GPIO_PE4_U5RX);
	GPIOPinConfigure(GPIO_PE5_U5TX);

	//RX, TX to GPS
	GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
}
void enable_GPS()
{
	//Initialize the UART.
	//Baud rate - Data bits - turn off parity - stop bits --- 9600 - 8 - Off - 1
	UARTConfigSetExpClk(b5, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

	UARTEnable(b5);
	//---------- GPS settings ----------//
	//coldStartReset();
	hotStartReset();
	enableSBAS();
	disableGGA();
	disableGLL();
	disableGSV();
	disableGSA();
	//---------- GPS settings ----------//

}
char *get_datastring()
{
	return data_string;
}
void listen_GPS()
{
	int i;
	char temp;
	//Wait for a new sentence
	while((char) UARTCharGet(b5) != '$'){}
	//Pass the information to the string
		for(i = 0; temp != '$' && i < 100; i++){
			temp = (char) UARTCharGet(b5);
			data_string[i] = temp;
		}

		//These functions check for the correct sentence type at the beginning of their execution
		//Runs with RMC
		parseTime();
		//Runs with VTG
		parseVelocity();
		//Runs with RMC
		parseLatitude();
		//Runs with RMC
		parseLongitude();


}

void parseTime()
{
	if((data_string[2] == 'R') && (data_string[3] == 'M') && (data_string[4] == 'C')){
	//If the string contains RMC data, place the time into UTCTime
		if(data_string[6] == ','){
			UTCTime[0] = '0';
			UTCTime[1] = '0';
			UTCTime[3] = '0';
			UTCTime[4] = '0';
			UTCTime[6] = '0';
			UTCTime[7] = '0';
		}
		else{
			UTCTime[0] = data_string[6];
			UTCTime[1] = data_string[7];
			UTCTime[3] = data_string[8];
			UTCTime[4] = data_string[9];
			UTCTime[6] = data_string[10];
			UTCTime[7] = data_string[11];
		}
	}
}

void parseVelocity(){
	if((data_string[2] == 'V') && (data_string[3] == 'T') && (data_string[4] == 'G')){

		//We have the VTG sentence that contains speed after the seventh comma
		int commaCounter = 0;
		int speedIndex;
		//Count the index at the speed reading
		for(speedIndex = 0; commaCounter < 7; speedIndex++){
			//If there is a comma, count until 7
			if(data_string[speedIndex] == ','){
				commaCounter++;
			}
		}
		//Current speed index is at the comma, we add one to check the next numeric value
		speedIndex++;
		int i,k;
		//If the character at speedIndex is a comma, there is no data available
		if(data_string[speedIndex] == ','){
			for(i = 0; i < 6; i++){
				velocityReading[i] = '0';
			}
		}
		//Else, we write the speed value to the velocityReading string
		else{
			for(k = 0; data_string[speedIndex + k] != ','; k++){
				velocityReading[k] = data_string[speedIndex + k];
			}
		}
	}
}

char *getVelocity(){
	//int speedInteger = atoi(velocityReading);//integer in knots
	// float speedMPH = 1.15 * speedInteger;
	// sprintf(speed, "%fMPH", speedMPH);
	return velocityReading;
}

char *getTime()
{
	return UTCTime;
}

void parseLatitude(){
	//After third comma
	if((data_string[2] == 'R') && (data_string[3] == 'M') && (data_string[4] == 'C')){
		int commaCounter = 0;
		int latitudeIndex = 0;
		for(latitudeIndex = 0; commaCounter < 3; latitudeIndex++){
			if(data_string[latitudeIndex] == ','){
				commaCounter++;
			}
		}
		latitudeIndex++;
		int i, k;
		if(data_string[latitudeIndex] == ','){
			for(i = 0; i < 11; i++){
				latitude[i] = '0';
			}
		}
		else{
			for(k = 0; data_string[latitudeIndex + k]; k++){
				latitude[k] = data_string[latitudeIndex + k];
			}

		}
	
	}
}

char *getLatitude(){
	return latitude;
}

void parseLongitude(){
	//After fifth
	if((data_string[2] == 'R') && (data_string[3] == 'M') && (data_string[4] == 'C')){
		int commaCounter = 0;
		int longitudeIndex = 0;
		for(longitudeIndex = 0; commaCounter < 5; longitudeIndex++){
			if(data_string[longitudeIndex] == ','){
				commaCounter++;
			}
		}
		longitudeIndex++;
		int i, k;
		if(data_string[longitudeIndex] == ','){
			for(i = 0; i < 12; i++){
				longitude[i] = '0';
			}
		}
		else{
			for(k = 0; data_string[longitudeIndex + k]; k++){
				longitude[k] = data_string[longitudeIndex + k];
			}

		}
	
	}
}

char *getLongitude(){
	return longitude;
}

void genericReader(){
	char_check = (char) UARTCharGet(b5);
	
}




/****************************************************************************************************************************/
//SBAS enable and disable
void enableSBAS(){
	int i;
	char enableSBASCommand[16] = {0xB5, 0x62, 0x06, 0x10, 0x08, 0x00, 0x01, 0x03, 0x03, 0x00, 0x51, 0x62, 0x06, 0x00, 0xE3, 0x27};
	for(i = 0; i < 16; i++){
		UARTCharPut(b5, enableSBASCommand[i]);
	}
}
void disableSBAS(){
	int i;
	char disableSBASCommand[16] = {0xB5, 0x62, 0x06, 0x10, 0x08, 0x00, 0x01, 0x03, 0x03, 0x00, 0x51, 0x62, 0x06, 0x00, 0xE4, 0x2F};
	for(i = 0; i < 16; i++){
		UARTCharPut(b5, disableSBASCommand[i]);
	}
}

//Cold and hot reset, takes into account previous session's ephemeris
void coldStartReset(){	
	int i;
	char cold_start_reset[12] = {0XB5, 0X62, 0X06, 0X04, 0X04, 0X00, 0XFF, 0X87, 0X02, 0X00, 0X96, 0XF9};
	for(i = 0; i < 16; i++){
		UARTCharPut(b5, cold_start_reset[i]);
	}
}
void hotStartReset(){
	int i;
	char hot_start_reset[12] = {0XB5, 0X62, 0X06, 0X04, 0X04, 0X00, 0X00, 0X00, 0X02, 0X00, 0X10, 0X68};
	for(i = 0; i < 16; i++){
		UARTCharPut(b5, hot_start_reset[i]);
	}
}

//Disables RMC, we need RMC to get time and velocity and is enabled defaultly
void disableRMC(){
	
	int i;
	char rmc_data_disable[16] = {0XB5, 0X62, 0X06, 0X01, 0X08, 0X00, 0XF0, 0X04, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X04, 0X40}; //RMC is the one needed
	for(i = 0; i < 16; i++){
		UARTCharPut(b5, rmc_data_disable[i]);
	}
}

//Disable unneeded NMEA sentences
void disableGGA(){
	int i;
	char gga_data_disable[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x24};
	for(i = 0; i < 16; i++){
		UARTCharPut(b5, gga_data_disable[i]);
	}


}
void disableGLL(){
	int i;
	char gll_data_disable[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2b};
	for(i = 0; i < 16; i++){
		UARTCharPut(b5, gll_data_disable[i]);
	}

}
void disableGSA(){
	int i;
	char gsa_data_disable[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x32};
	for(i = 0; i < 16; i++){
		UARTCharPut(b5, gsa_data_disable[i]);
	}


}
void disableGSV(){
	int i;
	char gsv_data_disable[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39};
	for(i = 0; i < 16; i++){
		UARTCharPut(b5, gsv_data_disable[i]);
	}
}
void disableVTG(){
	int i;
	char vtg_data_disable[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x47};
	for(i = 0; i < 16; i++){
		UARTCharPut(b5, vtg_data_disable[i]);
	}
}

