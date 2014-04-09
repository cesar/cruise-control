#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "lcd/ser_lcd.h"
#include "tmp36/tmp36.h"
#include "gps/gps635.h"
#include "microsd/microSD.h"
#include "microsd/fatfs/diskio.h"
#include "microsd/fatfs/ff.h"
#include "microsd/fatfs/ffconf.h"
#include "microsd/fatfs/integer.h"
#include "driverlib/rom.h"
#include "driverlib/ssi.h"


int main(void)
{

	setup_LCD();
	setup_tmp36();
	setup_GPS();
	setup_microSD();


	open_datalog();
	enable_LCD();
	enable_GPS();
	putPhrase("Hello Luis!");

	selectLineTwo();
	putPhrase("Temp: ");
	int i = get_analog_temp();
	char temp[2];
	sprintf(temp, "%i", i);
	putPhrase(temp);
	write_datalog("32MPH", "23", "31", "11:52PM", temp, "38");

	// listen_GPS();
	// char *gps_time = getTime();
	// putPhrase(gps_time);

	close();



}