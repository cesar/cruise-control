//============================================================
//                                                                     EXCERCISE 2
//============================================================
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

uint8_t ui8PinData=0x08; //Note: To enable X pin, data to be sent needs to be 2^x in hexadecimal.
uint8_t pin_0 = 0x1, pin_1 = 0x2,pin_2 = 0x4, pin_3 = 0x8, pin_4 = 0x10, pin_5 = 0x20, pin_6 = 0x40, pin_7 = 0x80;

void sevenSegWrite();
void writeToPins();

int main(void) {
  SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
  //Enable the peripherals
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  
  //Set the input type
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2);
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_6);  
  
  int offset[] = {0,1,1,1,1,1,1,1,1,1, 8, 1, 1, 1, 1, 1, 1};
  int i = 0;
  char letter = '0';

  while(1) {
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0x00);
    sevenSegWrite(letter);
    SysCtlDelay(13333333);
    letter = letter + offset[i];
    if (i == 16) {
      i = i % 16;
      letter = '0';
    }
    else {
      i ++;
    }
  }
}


void sevenSegWrite(char letter) {

  int char_index = (letter - 'A') + 65;

  //If the number is between 0 - 9
  if(char_index < 58 && char_index > 47) {
    char_index = char_index - 48;
  }

  //If the number is between A - F
  else if(char_index > 64 && char_index < 71) {
    char_index = char_index - 55;
  }
  
  int lookUpTable[16][7] = {
    {0,1,1,1,1,1,1}, //0
    {0,0,0,0,1,1,0}, //1
    {1,0,1,1,0,1,1}, //2
    {1,0,0,1,1,1,1}, //3
    {1,1,0,0,1,1,0}, //4
    {1,1,0,1,1,0,1}, //5
    {1,1,1,1,1,0,1}, //6
    {0,0,0,0,1,1,1}, //7
    {1,1,1,1,1,1,1}, //8
    {1,1,0,0,1,1,1}, //9
    {1,1,1,0,1,1,1}, //A - 10
    {1,1,1,1,1,1,1}, //B - 11
    {0,1,1,1,0,0,1}, //C - 12
    {0,1,1,1,1,1,1}, //D - 13
    {1,1,1,1,0,0,1}, //E - 14
    {1,1,1,0,0,0,1}  //F - 15
  };
  writeToPins(lookUpTable[char_index]);
}

void writeToPins(int *numbers) {

  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, pin_2 * numbers[2] + pin_3 *numbers[3]);
  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, pin_6 * numbers[5]);
  GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7) , pin_4 * numbers[4] + pin_5 * numbers[0] + pin_6 * numbers[1] + pin_7 * numbers[6]);
}

//============================================================
//                                                                     EXCERCISE 3
//============================================================



int frequency = 6;
uint8_t pin_0 = 0x1, pin_1 = 0x2,pin_2 = 0x4, pin_3 = 0x8, pin_4 = 0x10, pin_5 = 0x20, pin_6 = 0x40, pin_7 = 0x80;
int first_digit = 0xF0, second_digit = 0x0F, number = 0x00;

int main(void) {
  
  //Set the clock
  SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);


  //Enable the peripherals
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  
  //Set the input type
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2);
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_6);  

//Timer Configuration
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  TimerLoadSet(TIMER1_BASE, TIMER_A, 50000);
  TimerEnable(TIMER1_BASE, TIMER_A);

    
  while(1) {
    
    //Check the state of the timer
    if(TimerValueGet(TIMER1_BASE, TIMER_A) == 0x0) {
      number++;
      //reanable the timer.
      TimerLoadSet(TIMER1_BASE, TIMER_A, 50000);
    }
    
    
    int first_nible = (number & first_digit) >> 4;
    int second_nible = (number & second_digit);
    
    if(first_nible >= 10) {
      first_nible = first_nible + 7;
    } 

    if(second_nible >=10) {
      second_nible = second_nible + 7;
    }
    
    //Turn off both displays
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, pin_4);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, pin_2);
    
    //Make sure all data pins are low
    lower_pins();
 
    //Send Data
    sevenSegWrite((char)(((int)'0')+first_nible)); 
    

    //Turn on first 7-seg control signal
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x00);

    timer_delay();



    //Turn off control signal for both 7-seg
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, pin_4);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, pin_2);
    
    //Make sure all data pins are low
    lower_pins();

    //Send Data
    sevenSegWrite((char)(((int)'0')+second_nible));

    //Turn on second 7-seg control signal
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0x00);

    timer_delay();
}





}

void timer_delay()
{
  TimerLoadSet(TIMER0_BASE, TIMER_A, frequency);//(SysCtlClockGet()/60)/2);
  TimerEnable(TIMER0_BASE, TIMER_A);

  while(1)
  {

    if(TimerValueGet(TIMER0_BASE, TIMER_A) == 0x0)
    {
      break;
    }
  }
  return;
}

void lower_pins(){
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0x00);
  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,  0x00);
  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0x00);
  
  return;
}

void one_second(void) {
  TimerIntClear(TIMER1_BASE, TIMER_RTC_MATCH);
  //number++;
  return;
}

//============================================================
//                                                                     EXCERCISE 4
//============================================================


int frequency = 60, frecuency2 = 5000;
uint8_t pin_0 = 0x1, pin_1 = 0x2,pin_2 = 0x4, pin_3 = 0x8, pin_4 = 0x10, pin_5 = 0x20, pin_6 = 0x40, pin_7 = 0x80;
int first_digit = 0xF0, second_digit = 0x0F, number = 0x00;
char lookup_table[] = {'x','1','2','3','x','4', '5', '6','x', '7','8','9','x', '*','0', '#'};
char key_char = 'x', first_char = '0', second_char = '0';
int log_code = 0, code = 0, index_code = 0, counter = 0, temp, flag = 0;

int main(void) {
  
  //Set the clock
  SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);


  //Enable the peripherals
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  
  //Set the input type
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2);
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_6); 
  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5 | GPIO_PIN_4);
  GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_4);
  GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);


//Timer Configuration
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
  TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  TimerControlEvent(TIMER1_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  TimerControlEvent(TIMER2_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  
  // //Timer for the buttons
  TimerLoadSet(TIMER2_BASE, TIMER_A, frecuency2);
  TimerEnable(TIMER2_BASE, TIMER_A);
  TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
  TimerIntRegister(TIMER2_BASE, TIMER_A, timer_interrupt);

//Timer for the counter
  TimerLoadSet(TIMER1_BASE, TIMER_A, 50000);
  TimerEnable(TIMER1_BASE, TIMER_A);

    
  while(1) {
    
    //Turn off both displays
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, pin_4);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, pin_2);
    
    //Make sure all data pins are low
    lower_pins();
 
    //Send Data
    sevenSegWrite(first_char); 
    

    //Turn on first 7-seg control signal
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x00);

    timer_delay();



    //Turn off control signal for both 7-seg
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, pin_4);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, pin_2);
    
    //Make sure all data pins are low
    lower_pins();

    //Send Data
    sevenSegWrite(second_char);

    //Turn on second 7-seg control signal
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0x00);

    timer_delay();


//     //Buttons
    if(flag == 1) {
      //Reset the line read
      code = 0;

      //Turn on the scan for a line
      if(counter == 0) GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, pin_4);
      if(counter == 1) GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, pin_5);
      if(counter == 2) GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, pin_4);
      if(counter == 3) GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, pin_1);

      //Check which button on the line was pressed
      if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5) == pin_5) {
        code = 1;
        temp = log_code;
      }
      if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6) == pin_6) {
        code = 2;
        temp = log_code;
      }
      if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7) == pin_7) {
        code = 3;
        temp = log_code;
      }

      //Turn off the scan for a line
      if(counter == 0) GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0x0);
      if(counter == 1) GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x0);
      if(counter == 2) GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x0);
      if(counter == 3) GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0x0);
      
      //Check the next line on the next run and if overflows, reset
      

      //Calculate the index_code for the lookup table
      index_code = temp + code;
      key_char = lookup_table[index_code];
      log_code = log_code + 4;
      if(log_code > 12){
        log_code = 0;
      }

      counter++;
      if(counter > 3) counter = 0;
      if(key_char != 'x'){
        first_char = second_char;
        second_char = key_char;
        index_code = 0;
      }
      flag = 0;
      }
 }
}
void timer_interrupt(void)
{
  TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
  flag = 1;
}







