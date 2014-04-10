#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "driverlib/ssi.h"

#include "lcd/ser_lcd.h" //LCD Header File
#include "tmp36/tmp36.h" //Tmp36 Header
#include "gps/gps635.h" //GPS Header File

//Micro SD Includes
#include "microsd/microSD.h"
#include "microsd/fatfs/diskio.h"
#include "microsd/fatfs/ff.h"
#include "microsd/fatfs/ffconf.h"
#include "microsd/fatfs/integer.h"

int main(void)
{
	//========= Peripheral Enable and Setup ===========//
	setup_LCD(); //LCD Screen setup
	setup_tmp36(); //TMP36 ADC thermometer setup
	setup_GPS(); //GPS Pin Setup
	setup_microSD(); //MicroSD Pin Setup


	//========= Peripheral enable and run ============//
	open_datalog(); //Open microSD File for data logging
	enable_LCD(); //Start LCD Commmunication
	enable_GPS(); //Start GPS Communication


	selectLineOne();
	putPhrase("Temp: ");
	int i = get_analog_temp();
	char temp[2];
	sprintf(temp, "%i", i);
	putPhrase(temp);
	write_datalog("32MPH", "23", "31", "12:24AM", temp, "38");
	close();

	while(1){
		SysCtlDelay(1333333);

		selectLineTwo();

		listen_GPS();

		char *gps_time = getTime();
		putPhrase(gps_time);

		char *gps_speed = getVelocity();
		selectLineOne();
		putPhrase(temp);
		putPhrase(" ");
		putPhrase(gps_speed);
	}




}