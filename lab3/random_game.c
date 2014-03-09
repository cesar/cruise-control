#include <stdint.h> 
#include <stdbool.h> 
#include <string.h>
#include "inc/hw_memmap.h" 
#include "inc/hw_types.h" 
#include "driverlib/sysctl.h" 
#include "driverlib/gpio.h" 
#include "driverlib/timer.h"

// RS -> PC5
// E -> PC6
// DB7 -> PF4
// DB6 -> PA2
// DB5 -> PC4
// DB4 -> PA3
// DB3 -> PD6
// DB2 -> PA4
// DB1 -> PC7
// DB0 -> PE0

uint32_t port_A = GPIO_PORTA_BASE,
			port_C = GPIO_PORTC_BASE,
			port_D = GPIO_PORTD_BASE,
			port_E = GPIO_PORTE_BASE,
			port_F = GPIO_PORTF_BASE;

uint8_t pin_0 = 0x1, pin_1 = 0x2,pin_2 = 0x4, pin_3 = 0x8, pin_4 = 0x10, pin_5 = 0x20, pin_6 = 0x40, pin_7 = 0x80;

int user_guess = 0, //User's guess
	user_input = 0, //Flag for user input
	ask_user_input = 1,//Flag for asking user input
	game_mode_on = 0, //Flag for number check with random
	up_flag, down_flag = 0, //Flag for changes to the user's guess
	tries = 1, //Counts the user's tries.
	random_number; //The random number to be tested

int main() {


//Set the Clock at 16MHZ
SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

//Enable Timer Peripheral
SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

//Enable Peripherals
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); 
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); 
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);  
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);  
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  


//Initialize Output ports
GPIOPinTypeGPIOOutput(port_A, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4); 
GPIOPinTypeGPIOOutput(port_C, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); 
GPIOPinTypeGPIOOutput(port_D, GPIO_PIN_6); 
GPIOPinTypeGPIOOutput(port_E, GPIO_PIN_0); 
GPIOPinTypeGPIOOutput(port_F, (GPIO_PIN_4 | GPIO_PIN_1 | GPIO_PIN_3)); 

//Input Pins
GPIOPinTypeGPIOInput(port_F, GPIO_PIN_2 | GPIO_PIN_3);


//Interrupt Enables
GPIOIntEnable(GPIO_PORTF_BASE, (GPIO_INT_PIN_2 | GPIO_INT_PIN_3));
GPIOIntRegister(GPIO_PORTF_BASE, interrupt_handler);


GPIOIntTypeSet(GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3) , GPIO_FALLING_EDGE);



//Timer Setup
TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFF);
TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
TimerEnable(TIMER0_BASE, TIMER_A);

//Initialize the display
initializeDisplay();

random_number = get_random_number();

while(1)
{	
	//Displays the message to receive user input
	if(ask_user_input)
	{
		ask_user_input = 0;

		GPIOPinWrite(port_C, GPIO_PIN_5, pin_5);
		write_string("Input #: ");

	}

	//Increases the user guess by 1
	else if(up_flag)
	{	
		//Debouncing
		up_flag = 0;
		SysCtlDelay(160000);
		up_flag = 0;

		user_guess++;
		user_input = 1;

	}

	//Decreases the user guess by 1
	else if(down_flag)
	{	
		//Debouncing
		down_flag = 0;
		SysCtlDelay(160000);
		down_flag = 0;


		user_guess--;
		user_input = 1;
		if(user_guess < 0)
		{
			user_guess = 9;
		}
	}

	//Displays the user's number
	if(user_input)
	{
		user_input = 0;

		GPIOPinWrite(port_C, GPIO_PIN_5, pin_5);
		write_char_to_pins((char)(((int)('0')) + user_guess %10));

		GPIOPinWrite(port_C, GPIO_PIN_5, 0);
		writeDataPins(1,0,0,0,1,0,0,1);

	}

	//Activated when the user presses both buttons
	if(game_mode_on)
	{	
		game_mode_on = 0;
		 //RS Low
		GPIOPinWrite(port_C, GPIO_PIN_5, 0x0);

        //Next Display Line
		writeDataPins(1,1,0,0,0,0,0,0);

		 //RS High for writing
		GPIOPinWrite(port_C, GPIO_PIN_5, pin_5);
		if(tries == 3)
		{

			write_string("Game Over!  ");

			restart_game();
		}

		else
		{	

			if(user_guess == random_number)
			{
				write_string("You Win!");

				restart_game();
			}
			else
			{	

				write_string("Try Again...");
				GPIOPinWrite(port_C, GPIO_PIN_5, 0x0);
				writeDataPins(1,0,0,0,0,0,0,0);

				ask_user_input = 1;
				user_input = 1;
				user_guess = 0;
				tries++;
			}

			

		}
	}

	//RS Low for reset writing
	GPIOPinWrite(port_C, GPIO_PIN_5, 0x0);

}

}

//Restarts game condition
void restart_game()
{
	SysCtlDelay(27000000);
	GPIOPinWrite(port_C, GPIO_PIN_5, 0x0);

	clear_screen();
	random_number = get_random_number();
	ask_user_input = 1;
	user_input = 1;
	user_guess = 0;
	tries = 1;
}

//Returns a random value from the timer
int get_random_number()
{
	return (0x00FF & TimerValueGet(TIMER0_BASE, TIMER_A))% 10;

}

//Writes strings to the screen
void write_string(char *string)
{

	int i = 0;
	for(i = 0; i < strlen(string) ; i++)
	{
		write_char_to_pins(string[i]);
	}
}

//Writes to the data bus
void writeDataPins(uint8_t db7, uint8_t db6, uint8_t db5, uint8_t db4, uint8_t db3, uint8_t db2, uint8_t db1, uint8_t db0) {

	//DB7
	GPIOPinWrite(port_F, GPIO_PIN_4, db7 * pin_4);

	//DB6
	GPIOPinWrite(port_A, GPIO_PIN_2, db6 * pin_2);

	//DB5
	GPIOPinWrite(port_C, GPIO_PIN_4, db5 * pin_4);

	//DB4
	GPIOPinWrite(port_A, GPIO_PIN_3, db4 * pin_3);

	//DB3
	GPIOPinWrite(port_D, GPIO_PIN_6, db3 * pin_6);

	//DB2
	GPIOPinWrite(port_A, GPIO_PIN_4, db2 * pin_4);

	//DB1
	GPIOPinWrite(port_C, GPIO_PIN_7, db1 * pin_7);

	//DB0
	GPIOPinWrite(port_E, GPIO_PIN_0, db0 * pin_0);

	//Turn On The Clock to activate the commmand
	enableHL();

}

//Toggle of the enable line
void enableHL() {

	//Enable high
	GPIOPinWrite(port_C, GPIO_PIN_6, pin_6);

	//Wait 5ms
	SysCtlDelay(130000);

	//Enable low
	GPIOPinWrite(port_C, GPIO_PIN_6, 0x0);

	//Wait 5ms
	SysCtlDelay(130000);
}


//Display Initialization
void initializeDisplay() {

	//RS low
	GPIOPinWrite(port_C, GPIO_PIN_5, 0x0);

	//Function Set
	writeDataPins(0,0,1,1,1,0,0,0);
	//Clear the screen
	clear_screen();

	//Display on/off control
	set_cursor();  

	//Entry mode set
	writeDataPins(0,0,0,0,0,1,1,0);
}

void set_cursor()
{
	//Display on/off control
	writeDataPins(0,0,0,0,1,1,1,0);
}

//Clear the screen
void clear_screen()
{
	//Clear Display
	writeDataPins(0,0,0,0,0,0,0,1);
}

//Convert a char and output into data pins
void write_char_to_pins(char letter)
{
	//Convert the letter into binary
	int char_index = (letter- 'A') + 65;
	int binary[8];
	int i = 0;
	for(i = 0; i < 8; i ++)
	{
		binary[i] = char_index %2;
		char_index = char_index/2;
	}

	//Write corresponding data to the pins with respect to the letters
	writeDataPins(binary[7], binary[6], binary[5], binary[4], binary[3], binary[2], binary[1], binary[0]);

}


//Handles the interrupts for the game
void interrupt_handler(void)
{
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
	//Both switches
	if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x0 && GPIOPinRead(port_F, GPIO_PIN_3) == 0x0) { 
		game_mode_on = 1;

	} 

	//Up Switch
	else if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x0) { 
		up_flag = 1;

	} 

	//Down Switch
	else if(GPIOPinRead(port_F, GPIO_PIN_3) == 0x0) { 
		down_flag = 1;

	} 

}

