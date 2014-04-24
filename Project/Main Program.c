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
int set_velocity, enableSys = 0;
int incr_speed, decr_speed, obtain_speed;
char digi_temp[3];
char temp[3];




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
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0);

	GPIOIntEnable(GPIO_PORTF_BASE, (GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_4));
	GPIOIntRegister(GPIO_PORTF_BASE, bHandler);
	GPIOIntTypeSet(GPIO_PORTF_BASE, (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_INT_PIN_4) , GPIO_FALLING_EDGE);


	GPIOIntRegister(GPIO_PORTB_BASE, brakeHandler);
	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0);
	GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);

	//========= Peripheral enable and run ============//
	enable_LCD(); //Start LCD Commmunication
	enable_GPS(); //Start GPS Communication
	while(1)
	{	
		

		//Wait one second
		SysCtlDelay(SysCtlClockGet() * 2/3);
		if(enableSys)
		{	
			if(obtain_speed)
			{
				clearDisplay();

				//Get current speed
				//set_velocity = getVelocity();
				SysCtlDelay(SysCtlClockGet() * 0.020/3);
				obtain_speed = 0;
				set_velocity = 50;
			}

			if(incr_speed)
			{	
				SysCtlDelay(SysCtlClockGet() * 0.020/3);
				incr_speed = 0;
				SysCtlDelay(SysCtlClockGet() * 0.020/3);
				set_velocity += 1;



			}
			if(decr_speed)
			{
				SysCtlDelay(SysCtlClockGet() * 0.020/3);
				decr_speed = 0;
				SysCtlDelay(SysCtlClockGet() * 0.020/3);
				set_velocity -= 1;
			}

			setup_microSD();
			open_datalog();

			//Listen for the GPS Data
			listen_GPS();

			//Select Line One
			selectLineOne();

			//Outside GPS Enability
			// char* velocity;
			// velocity = getVelocity();
			char velocity[6];
			sprintf(velocity, "%imph", set_velocity);
			putPhrase("Vel: ");
			putPhrase(velocity);

			//Print velocity in LCD
			write_datalog(velocity, "", "", "", temp, digi_temp);
			close();
		}
		else
		{
			SysCtlDelay(SysCtlClockGet() * 0.020/3);
			clearDisplay();
			selectLineOne();
			putPhrase("System Standby");
		}

		selectLineTwo();

		int i = get_analog_temp();
		sprintf(temp, "%i", i);

		int j = getTemperature();
		sprintf(digi_temp, "%i", j);

			//Print temperature to LCD
		putPhrase("M: ");
		putPhrase(temp);
		putPhrase(", ");

			//Print digital temperature to LCD
		putPhrase("E: ");
		putPhrase(digi_temp);
	}
}

void brakeHandler(){

	GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0);

	if(GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0) == 0x0)
	{
		enableSys = 0;
	}

}

void bHandler(void) {

	GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_4);

    //Increase velocity
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) == 0x0) {
		incr_speed = 1;
	}
    //Decrease velocity
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) == 0x0) {
		decr_speed = 1;
	}
    //Enable Disable
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x0) {
		
		if(enableSys == 1) {
			enableSys = 0;
		}
		else {
			enableSys = 1;
			obtain_speed = 1;
		}
	}
}
