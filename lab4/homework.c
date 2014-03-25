#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"


//Using PB3 as the Output Pin


int frequency = 60;
uint8_t pin_0 = 0x1, pin_1 = 0x2,pin_2 = 0x4, pin_3 = 0x8, pin_4 = 0x10, pin_5 = 0x20, pin_6 = 0x40, pin_7 = 0x80;
int first_digit = 0xF0, second_digit = 0x0F, number = 0x00;
char lookup_table[] = {'x','1','2','3','x','4', '5', '6','x', '7','8','9','x', 'A','0', 'B'};
char key_char = 'x', first_char = '0', second_char = '0';
int log_code = 0, code = 0, index_code = 0, counter = 0, temp, flag = 0, calc_counter, reset = -1, first_value, second_value, result, enter_flag = 1;
void toggle();
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
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  
  //Set the input type
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2);
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_6 ); 
  GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_2);
  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5 | GPIO_PIN_4);
  GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_4);
  GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

  //Set interrupts
  GPIOIntEnable(GPIO_PORTD_BASE, (GPIO_INT_PIN_2 | GPIO_INT_PIN_3));
  GPIOIntRegister(GPIO_PORTD_BASE, interrupt_handler);
  GPIOIntTypeSet(GPIO_PORTD_BASE, (GPIO_PIN_2 | GPIO_PIN_3) , GPIO_FALLING_EDGE);

//Timer Configuration
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  TimerControlEvent(TIMER2_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  
  // //Timer for the buttons
  TimerLoadSet(TIMER2_BASE, TIMER_A, 1600000);
  TimerEnable(TIMER2_BASE, TIMER_A);
  TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
  TimerIntRegister(TIMER2_BASE, TIMER_A, timer_interrupt);


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


     //Buttons
    if(flag == 1){
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
    }//end flag == 1 if statement

    if(calc_counter == 1 && enter_flag == 0) {
      first_value = second_char - '0';
      toggle();
      enter_flag = 1;
    }

    else if(calc_counter == 2 && enter_flag == 0) {
      second_value = second_char - '0';
      toggle();
      enter_flag = 1;
    }

    else if(calc_counter == 3 && enter_flag == 0){
      toggle();
      enter_flag = 1;
      if(second_char == 'A'){
        result = first_value + second_value;
      }

      else if(second_char == 'B') {
        if(second_value > first_value) {
          result = second_value - first_value;
        }
        else if(second_value < first_value) {
          result = first_value - second_value;
        }
        
      }
      if(result > 9) {
            //Break number in two
          second_char= (char)(((int)'0')+ (result % 10));
          if(result < 0) result = result * -1;
          first_char = (char)(((int)'0')+ (result / 10.0));

        }
      else if (result < 10) {
        second_char = (char)(((int)'0')+ (result));
        first_char = '0';
      }
      calc_counter = 0;
    }

    if(reset == 1) {
      toggle();
      calc_counter = 0;
      reset = -1;
      first_value = second_value = 0;
      first_char = second_char = '0';
    }

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

void add_numbers() {

}

void substract_numbers() {

}

void lower_pins(){
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0x00);
  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,  0x00);
  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0x00);
  
  return;
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

void timer_interrupt(void)
{
  TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
  flag = 1;
}

void interrupt_handler(void) {
  GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_2 | GPIO_INT_PIN_3);

  if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2) == 0x00){
      //Enter
    calc_counter++;
    enter_flag = 0;
    SysCtlDelay(1000000);
  }

  if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3) == 0x00) {
      //Clear
    reset = 1;
    SysCtlDelay(1000000);
  }
}

void toggle(){
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
  SysCtlDelay(300000);
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
}



