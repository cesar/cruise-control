#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/timer.h"

#include "serlcd/ser_lcd.h" //LCD Header File
#include "tmp36/tmp36.h" //Tmp36 Header
#include "tmp102/tmp102.h" //Tmp102 Header
#include "gps/gps635.h" //GPS Header File
#include "pwm_module/pwm_module.h" //PWM Header File
#include "pid/pid.h" //PID Module
#include "bms/bms.h" //Battery Management System

//Micro SD Includes
#include "microsd/microSD.h"
#include "microsd/fatfs/diskio.h"
#include "microsd/fatfs/ff.h"
#include "microsd/fatfs/ffconf.h"
#include "microsd/fatfs/integer.h"

//Global variables
float set_velocity, //Set point velocity variable
 enableSys = 0,//System enable flag
 incr_speed,//Increase speed flag
 decr_speed, //Decrease speed flag
 obtain_speed; //Start cruise control speed flag
char digi_temp[3]; //Digital temperature
char anag_temp[3];	 //Analog temperature
SPid pid;	//Pid Data structure
float scaleFactor = 100; //Determined by the motor
uint32_t period; //Timer interrupt

void bHandler(void);

int main(void)
{
	//========= Peripheral Enable and Setup ===========//
	setup_LCD(); //LCD Screen setup
	setup_tmp36(); //TMP36 ADC thermometer setup


	setup_GPS(); //GPS Pin Setup
	setup_microSD(); //MicroSD Pin Setup
	setupTMP102();  
	setup_PID(&pid);
	setupBMS();
	setup_pwm();

	//Setup the buttons
	
	setup_buttonInterrupts();
	sleepEnablePeripherals();
	setup_timerInterrupt();

	
	//========= Peripheral enable and run ============//
	enable_LCD(); //Start LCD Commmunication
	enable_GPS(); //Start GPS Communication
	clearDisplay(); //Refresh Display



	while(1)
	{	
		//Listen for the GPS Data
		listen_GPS();
		//Open the datalog file for writing
		open_datalog();

		//Obtain vehicle speed
		char *velocity;
		char velocityArray[] = "000";
		
		//velocity = velocityArray;
		velocity = getVelocity();
		//Convert string to int
		int speedInteger = atoi(velocity);
		//Convert knots to mph

		
		speedInteger = 1.15 * speedInteger;
		//Return to string
		sprintf(velocity, "%i", speedInteger);
		// velocity = "50";
		selectLineOne();
		//Show velocity
		putPhrase("V:");
		putPhrase(velocity);

		//Analog Temperature Sensor
		int anag_tempValue = get_analog_temp();
		sprintf(anag_temp, "%i", anag_tempValue);

		//Digital Temperature Sensor
		int digi_tempValue = getTemperature();
		sprintf(digi_temp, "%i", digi_tempValue);

		//Get BMS Level
		char load[] = "89";
		char *load_value;
		load_value = load;
		//-------BMS Communication------
		sendStartSequence();
		getDataString();
		load_value = getLoad();
		//------------------------------

		

			
		//If Cruise Control is on
		if(enableSys)
		{	
			//If initialized
			if(obtain_speed)
			{	//Debouncing
				//SysCtlDelay(533333);
				obtain_speed = 0;
				clearDisplay();

				//Get set velocity
				set_velocity = 10;
			}

			//If driver increases set speed
			if(incr_speed)
			{	
				//SysCtlDelay(533333);

				incr_speed = 0;
				set_velocity += 1;
			}

			//if driver decresaes set speed
			else if(decr_speed)
			{	
				//SysCtlDelay(533333);
				decr_speed = 0;
				set_velocity -= 1;
			}

			//==============PID Portion of the Main Loop ==============//
			float process_value = atof(velocity); //Convert String to Int
			float error = set_velocity - process_value; //Calculate error
			float u_t = UpdatePID(&pid, error, 1.0); //Feed error to the PID
			
			uint32_t duty_cycle = u_t*scaleFactor;
			if(duty_cycle < 950){
				duty_cycle = 50;
			}
			else if(duty_cycle > 0){
				duty_cycle = 950;
			}
			pwm_out(1000, duty_cycle); //Scale and output the PID output

			//====================================================//

			//Show other essentials to the LCD
			putPhrase("mph/"); 

			char set_point[5];
			sprintf(set_point, "%imph", (int) set_velocity);
			putPhrase(set_point);
			putPhrase(" ON"); //Cruise control on

		}

		else
		{
			//Spacer
			putPhrase("mph ");
			putPhrase("Standby  ");

		}

		//Select bottom line
		selectLineTwo();

		//Display analog and digital temperatures
		putPhrase("T:");
		putPhrase(anag_temp);
		putPhrase("/");
		putPhrase(digi_temp);
		putPhrase("C  ");


		//Show Load level
		putPhrase("B:");
		putPhrase(load_value);
		putPhrase("%");

		
		write_datalog(velocity, "ddmm.mmmmmmX", "ddmm.mmmmmX", getTime(), anag_temp, digi_temp, load_value);
		close();

		//Put system in low power mode
		SysCtlSleep();	


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

    //Increase velocity is PF1
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) == 0x0) {
		incr_speed = 1;
	}
    //Decrease velocity is PF2
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) == 0x0) {
		decr_speed = 1;
	}
    //Enable Disable is PF4
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

void sleepEnablePeripherals(){

	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOB);

  	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOC);
  	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOD);
  	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOE);


  	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_SSI0);
  	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_I2C1);
  	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_I2C2);
  	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART5);

	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART3);

	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER1);


 	SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOF);
  	SysCtlPeripheralClockGating(true);


}
void setup_buttonInterrupts(){

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0);

	GPIOIntEnable(GPIO_PORTF_BASE, (GPIO_INT_PIN_1 | GPIO_INT_PIN_2 | GPIO_INT_PIN_4));
	GPIOIntRegister(GPIO_PORTF_BASE, bHandler);
	GPIOIntTypeSet(GPIO_PORTF_BASE, (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_INT_PIN_4) , GPIO_FALLING_EDGE);

	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0);
	GPIOIntRegister(GPIO_PORTB_BASE, brakeHandler);
	GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);

}

void setup_timerInterrupt(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	period = (SysCtlClockGet());
	//Timer Configuration
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);


    TimerLoadSet(TIMER1_BASE, TIMER_A, period);
    TimerEnable(TIMER1_BASE, TIMER_A);

    //Timer Interrupt Enable
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER1_BASE, TIMER_A, timer_interrupt);
	
}

void timer_interrupt()
{
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

}