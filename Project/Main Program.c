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
#include "tmp102/tmp102.h" //Tmp102 Header
#include "gps/gps635.h" //GPS Header File
// #include "pwm_module/pwm_module.h" //PWM Header File

//Micro SD Includes
#include "microsd/microSD.h"
#include "microsd/fatfs/diskio.h"
#include "microsd/fatfs/ff.h"
#include "microsd/fatfs/ffconf.h"
#include "microsd/fatfs/integer.h"

//Global variables
int velocity, enableSys;

void bHandler(void);

int main(void)
{
	//========= Peripheral Enable and Setup ===========//
	setup_LCD(); //LCD Screen setup
	setup_tmp36(); //TMP36 ADC thermometer setup
	setup_GPS(); //GPS Pin Setup
	setup_microSD(); //MicroSD Pin Setup
	setupTMP102(); //TMP102 DIgital Thermometer Setup

	//Setup the buttons
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4);
	GPIOIntEnable(GPIO_PORTF_BASE, (GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_4));
	GPIOIntRegister(GPIO_PORTF_BASE, bHandler);
	GPIOIntTypeSet(GPIO_PORTF_BASE, (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_INT_PIN_4) , GPIO_RISING_EDGE);


	//========= Peripheral enable and run ============//
	open_datalog(); //Open microSD File for data logging
	enable_LCD(); //Start LCD Commmunication
	enable_GPS(); //Start GPS Communication
	char temp[3];
	while(1)
	{	
		setup_microSD();

		//Listen for the GPS Data
		listen_GPS();

		//Wait one second
		SysCtlDelay(SysCtlClockGet() * 1/3);
		//Select Line One
		selectLineOne();
	
		

		char* velocity;
		velocity = getVelocity();
		putPhrase("Vel: ");
		putPhrase(velocity);
		putPhrase("K");		


		selectLineTwo();

		int i = get_analog_temp();
		sprintf(temp, "%i", i);

		char digi_temp[3];
		int j = getTemperature();
		sprintf(digi_temp, "%i", j);
		//Print temperature to LCD
		putPhrase("M: ");
		putPhrase(temp);
		putPhrase(", ");

		//Print digital temperature to LCD
		putPhrase("E: ");
		putPhrase(digi_temp);

		//Print velocity in LCD
		// putPhrase(" V: ");
		// putPhrase(gps_speed);

		
		// char *gps_time = getTime();
		write_datalog(velocity, getLongitude(), getLatitude(), getTime(), temp, digi_temp);
		close();
	}
}
void lcd_test()
{
	clearDisplay();
	putPhrase("Hello World!");
}

// void setup_SleepMode()
// {
// 	//Setup the peripherals that are enabled in sleep mode
// 	 ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOF);
// 	 ROM_SysCtlPeripheralClockGating(true);

// }

void bHandler(void) {
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_4);

    //Increase velocity
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) == GPIO_PIN_1) {

    }
    //Decrease velocity
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) == GPIO_PIN_2) {

    }
    //Enable Disable
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == GPIO_PIN_4) {
   	if(enableSys == 1) {
        enableSys = 0;
      }
      else {
        enableSys = 1;
      }
    }
}
