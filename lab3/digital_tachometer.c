#include <stdint.h> 

#include <stdbool.h> 

#include <string.h>

#include <math.h>

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

uint32_t port_A = GPIO_PORTA_BASE;
uint32_t port_C = GPIO_PORTC_BASE;
uint32_t port_D = GPIO_PORTD_BASE;
uint32_t port_E = GPIO_PORTE_BASE;
uint32_t port_F = GPIO_PORTF_BASE;
uint8_t pin_0 = 0x1, pin_1 = 0x2,pin_2 = 0x4, pin_3 = 0x8, pin_4 = 0x10, pin_5 = 0x20, pin_6 = 0x40, pin_7 = 0x80;

void writeDataPins(uint8_t db7, uint8_t db6, uint8_t db5, uint8_t db4, uint8_t db3, uint8_t db2, uint8_t db1, uint8_t db0);

void enableHL();

void resetPins();

void initializeDisplay();

void write_char_to_pins(char letter);

void write_phrases();

void counter(int counter);

void interrupt_handler(void);

void scrolling_wheel(void);

void the_taco_meter(void);

void taco_display(void);

int SIZE = 16;
int current_list_position = 0;
int count = 0;
int flag = 1;
int frequency = 160000;
int current_state_a, current_state_b, previous_state_a = -1, previous_state_b = -1, time_a, time_b, direction;
int direction_change = 0, speed_change = 0, current_direction = 0;
float t1 = 0, t2 = 0, current_rpm, delta;
int initial_run = 1;
int main() {

  //Enable Peripherals
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

  
  //Start specific Pin Ports
  GPIOPinTypeGPIOOutput(port_A, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4); 
  GPIOPinTypeGPIOOutput(port_C, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); 
  GPIOPinTypeGPIOOutput(port_D, GPIO_PIN_6); 
  GPIOPinTypeGPIOOutput(port_E, GPIO_PIN_0); 
  GPIOPinTypeGPIOOutput(port_F, GPIO_PIN_4); 
  GPIOPinTypeGPIOInput(port_F, GPIO_PIN_2 | GPIO_PIN_3);
  
  //Timer Configuration
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerLoadSet(TIMER0_BASE, TIMER_A, frequency);
  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  TimerEnable(TIMER0_BASE, TIMER_A);

  
  //Enable pin for interrupt
  GPIOIntEnable(GPIO_PORTF_BASE, (GPIO_INT_PIN_2 | GPIO_INT_PIN_3));
  
  //Set ISR
  GPIOIntRegister(GPIO_PORTF_BASE, the_taco_meter);
  
  //Set interrupt type
  GPIOIntTypeSet(GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3) , GPIO_BOTH_EDGES);
  
  //Initialize the display
  initializeDisplay();
  
  //Write phrases
  //write_phrases();
  //Count how many times a button is pressed
  //RS High
  GPIOPinWrite(port_C, GPIO_PIN_5, pin_5);
  write_string("Speed = ### RPM");
  while(1) { taco_display(); }

}

void the_taco_meter(void) {

  //Clear
  GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_2 | GPIO_INT_PIN_3);

  int codex[] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};  
  int temp_binary;
  
  //Get a snapshot of the current state of the wheel positions
  if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x0 && GPIOPinRead(port_F, GPIO_PIN_3) == 0x0) {
    current_state_a = 0;
    current_state_b = 0;

  }
  else if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x4 && GPIOPinRead(port_F, GPIO_PIN_3) == 0x0) {
    current_state_a = 1;
    current_state_b = 0;

  }
  else if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x0 && GPIOPinRead(port_F, GPIO_PIN_3) == 0x8) {
    current_state_a = 0;
    current_state_b = 1;

  }
  else if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x4 && GPIOPinRead(port_F, GPIO_PIN_3) == 0x8) {
    current_state_a = 1;
    current_state_b = 1;

  }

  //Get the wheel direction
  if(previous_state_a != -1 && previous_state_b != -1) {
    //Convert binary to decimal and add to counter variable.
    //Proper order should be PrevA PrevB NewA NewB
    temp_binary = (pow(2, 3) * previous_state_a) + (pow(2, 2) * previous_state_b) + (pow(2, 1) * current_state_a) + (pow(2, 0) * current_state_b);
    
    if(codex[temp_binary] == 1) {
      //Direction is clockwise
      direction = 1;
    }

    else if(codex[temp_binary] == -1) {
      //Direction is counter-clockwise
      direction = -1;
    } 

    if(direction != current_direction)
    {
      direction_change = 1;
      current_direction = direction;
    }

    
      //jump initial value
      t1 = t2;
      //Get the current speed
      t2 = TimerGetValue(TIMER0_BASE, TIMER_A);
      //conversion
      t2 = t2 * (6.25) * pow(10, -8);
      delta = t2 - t1;
      if(delta < 0){
        delta +=60;
      }
      current_rpm = (1/24) / (delta);
      current_rpm = current_rpm / 60;

      GPIOPinWrite(port_F, GPIO_PIN_5, pin_5);
      write_char_to_pins((char)((int)'0') + (int) current_rpm);

    


    
  }

  flag = 1;

  previous_state_a = current_state_a;
  previous_state_b = current_state_b;
}

void change_direction_LCD(int direction)
{

    //RS Low
  GPIOPinWrite(port_C, GPIO_PIN_5, 0x0);

    //Next Display Line
  writeDataPins(1,1,0,0,0,0,0,0);

  if(current_direction == 1)
  {
    GPIOPinWrite(port_C, GPIO_PIN_5, pin_5);
    write_string("CWise  ");
  }

  if(current_direction == -1)
  {
    GPIOPinWrite(port_C, GPIO_PIN_5, pin_5);
    write_string("CCWise ");

  }
  GPIOPinWrite(port_C, GPIO_PIN_5, 0x0);


}

void taco_display(void) {

  //Write to LCD
  if (flag == 1) {


    if(direction_change)
    { 
      direction_change = 0;
      SysCtlDelay(160000);
      direction_change = 0;
      change_direction_LCD(direction);

    }

    if(speed_change)
    {
      GPIOPinWrite(port_F, GPIO_PIN_5, 0x0);
      writeDataPins(1,0,0,0,1,0,0,1);

      GPIOPinWrite(port_F, GPIO_PIN_5, pin_5);

        write_string(""); //Wrtie some speed
        writeDataPins(1,0,0,0,1,0,0,1);

      }




    }
    flag = 0;
  }




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

  // //Entry mode set
  // writeDataPins(0,0,0,0,0,1,1,0);
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


//Write the number of times a button has been pressed
 void counter(int counter) {

  //Write
  if(flag == 1) {

    flag = 0;
    
    //First clear the screen
    clear_screen();
    
    //Convert number to chars
    char number[16] = {};
    int i, current = counter, temp, len;
    for(i = 0; i < 16; i++ ) {
      temp = current % 10; //Give me the last digit
      current = current / 10; //Crop out last digit
      
      number[i] = (char)(temp + 48);
      
      if(current == 0) {
        len = i;
        break;
      }
    }
    
    
    for(i = len; i >= 0; i--) {

     //Switch RS to high
     GPIOPinWrite(port_C, GPIO_PIN_5, pin_5);

     write_char_to_pins(number[i]);
     
     //RS low
     GPIOPinWrite(port_C, GPIO_PIN_5, 0x0);
     
   } 

 }
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

//Writes strings to the screen
void write_string(char *string)
{

  int i = 0;
  for(i = 0; i < strlen(string) ; i++)
  {
    write_char_to_pins(string[i]);
  }
}
void interrupt_handler(void)
{
  GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
  
  if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x0) { 
    count++;
  } 
  else if(GPIOPinRead(port_F, GPIO_PIN_3) == 0x0) {
    count--;
  }
  counter(count);
  flag = 1;
}

void scrolling_wheel(void) {

  //Pin PF2 represents A and Pin PF3 represents B

  GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_2 | GPIO_INT_PIN_3);

  
  int codex[] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
  int temp_binary;
  
  
  //Get a snapshot of the current state of the wheel positions
  if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x0 && GPIOPinRead(port_F, GPIO_PIN_3) == 0x0) {
    current_state_a = 0;
    current_state_b = 0;
    //count = 1;
  }
  else if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x4 && GPIOPinRead(port_F, GPIO_PIN_3) == 0x0) {
    current_state_a = 1;
    current_state_b = 0;
    //count = 3;
  }
  else if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x0 && GPIOPinRead(port_F, GPIO_PIN_3) == 0x8) {
    current_state_a = 0;
    current_state_b = 1;
    //count = 4;
  }
  else if(GPIOPinRead(port_F, GPIO_PIN_2) == 0x4 && GPIOPinRead(port_F, GPIO_PIN_3) == 0x8) {
    current_state_a = 1;
    current_state_b = 1;
    //count = 2;
  }
  
  if(previous_state_a != -1 && previous_state_b != -1) {
    //Convert binary to decimal and add to counter variable.
    //Proper order should be PrevA PrevB NewA NewB
    temp_binary = (pow(2, 3) * previous_state_a) + (pow(2, 2) * previous_state_b) + (pow(2, 1) * current_state_a) + (pow(2, 0) * current_state_b);
    
    current_list_position = current_list_position + codex[temp_binary];
    
  }
  
  previous_state_a = current_state_a;
  previous_state_b = current_state_b;
  
  //Counter function
  //counter(count);
  
  //Phrases Function
  write_phrases();
  
  flag = 1;
}






