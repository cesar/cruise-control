#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/ssi.h"
#include "driverlib/uart.h"


#define read_hour 0x02
#define read_minutes 0x01
#define read_seconds 0x00
#define read_day 0x03
#define read_date 0x04
#define read_month 0x05
#define read_year 0x06

#define write_seconds 0x80
#define write_minutes 0x81
#define write_hour 0x82
#define write_days 0x83
#define write_date 0x84
#define write_month 0x85
#define write_year 0x86

#define AM 0x00
#define PM 0x20
#define _12HOUR 0x40
#define _24HOUR 0x00


//LCD Prototypes
void putChar(char character);
void clearDisplay();
void putPhrase(char *string);
void selectLineOne();
void selectLineTwo();
void changeCursorUnderscore();
void toggleLED();


//RTC Prototypes
void select();
void deselect();
void initialize_rtc();
void setup_peripherals();
uint32_t read(uint32_t cmd);
void write(uint32_t cmd, uint32_t argument);





char ser_lcd_command;
uint32_t b1 = UART1_BASE; 
uint32_t time[4];
char* days[] = {"Sunday", "Monday", "Tuesday",
 "Wednesday", "Thursday", "Friday", "Saturday"};

 char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
"Aug", "Sep", "Oct", "Nov", "Dec"};

int hour_type = 1;
void select()
{
  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x2);


}
void deselect()
{
  GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x0);
}

uint32_t BCD_to_Decimal(uint32_t bcd)

{
  return (((bcd & 0xF0) >> 4) * 10) + (bcd & 0x0F);
}

uint32_t Decimal_to_BCD(uint32_t decimal)
{
  return (((decimal / 10) << 4) | (decimal % 10));
}

void setup_peripherals()
{

 SysCtlClockSet(SYSCTL_SYSDIV_5| SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); 

  //Run the clock at 40MHz
 Serial.begin(9600);
 Serial.println("Starting");


 //==================SSI Module============//
  //Initialize the peripherals
 SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

  //Configure output pins
 GPIOPinConfigure(GPIO_PD0_SSI3CLK);
 GPIOPinConfigure(GPIO_PD2_SSI3RX);
 GPIOPinConfigure(GPIO_PD3_SSI3TX);
  //Set pin type to SSI
 GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3);
  //Configure CS
 GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);


 Serial.println("Peripherals Enabled. Configuring SSI Clock: ");
  //Clk configuration, bit rate, etc.
 SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, 96000, 8 );
 SSIEnable(SSI3_BASE);

   //==================UART Module============//
 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
 SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
 GPIOPinConfigure(GPIO_PB1_U1TX);
 GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
 GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_1);
  //Initialize the UART. UART is base 7 in PE1.
  //Baud rate - Data bits - turn off parity - stop bits --- 9600 - 8 - Off - 1
 UARTConfigSetExpClk(b1, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
 UARTEnable(b1);
 //changeCursorUnderscore();


 Serial.println("Clock configured.");

  //Enable SSI
 Serial.println("SSI Enabled");

}

uint32_t read(uint32_t cmd)
{   

  uint32_t response;

 //CS High - Select
  select();
  //Send Read Command
  SSIDataPut(SSI3_BASE,cmd);
  while(SSIBusy(SSI3_BASE));

  SSIDataPut(SSI3_BASE,cmd);
  while(SSIBusy(SSI3_BASE));

  while(SSIDataGetNonBlocking(SSI3_BASE,&response));
  deselect();

  return BCD_to_Decimal(response);

}

void write(uint32_t cmd, uint32_t argument)
{

 //CS High - Select
  select();

  //Send write command
  SSIDataPut(SSI3_BASE,cmd);

  //Send write data argument to registers
  SSIDataPut(SSI3_BASE,argument);

  while(SSIBusy(SSI3_BASE));

  deselect();
}

void initialize_rtc()
{
  write(0x8F,0x0);
}

void setup()
{ 

  setup_peripherals();
  initialize_rtc();
  
  set_time(11,59, 57, _12HOUR, AM);
  set_date(4, 3, 3, 14);

  while(1){

    SysCtlDelay(13333333);
    get_time();
    get_date();
  }
}

void get_time()
{
  time[0] = read(read_seconds);
  time[1] = read(read_minutes);
  uint32_t temp_hour = read(read_hour);
   Serial.println("");

  time[2] = BCD_to_Decimal(Decimal_to_BCD(temp_hour) & 0x1F);
  time[3] = BCD_to_Decimal((Decimal_to_BCD(temp_hour) >>4) & 0x2);

  Serial.print(time[2]);
  Serial.print(":");

  if(time[1] < 10) Serial.print("0");
  Serial.print(time[1]);
  Serial.print(":");

  if(time[0] < 10) Serial.print("0");
  Serial.print(time[0]);
  if(hour_type){
  if(time[3] == 0x2)Serial.print(" PM");
  if(time[3] == 0x0)Serial.print(" AM");
}
  Serial.println("");


}

void print_to_binary(uint32_t number)
{
  uint32_t temp = Decimal_to_BCD(number);
  for(int i = 0; i < 7; i++)
  {
    Serial.print((number >> i) & 0x01);
  }

}
void set_time(uint32_t hour, uint32_t minutes, uint32_t seconds, uint32_t hour_option, uint32_t AM_PM)
{
  write(write_hour, Decimal_to_BCD(hour) + AM_PM + hour_option);
  write(write_minutes, Decimal_to_BCD(minutes));
    write(write_seconds, Decimal_to_BCD(seconds));



}

void get_date()
{
  uint32_t date[4];
  date[0] = read(read_day);
  date[1] = read(read_date);
  date[2] = read(read_month);
  date[3] = read(read_year);

  Serial.print(days[date[0]]);
  Serial.print(", ");
  Serial.print(months[date[2]]);
  Serial.print( " ");
  Serial.print(date[1]);
  Serial.print(", ");

  Serial.print(date[3]);
    Serial.println("");





}


void set_date(uint32_t day,uint32_t date, uint32_t month, uint32_t year)
{
  write(write_days, Decimal_to_BCD(day));
  write(write_date, Decimal_to_BCD(date));
  write(write_month, Decimal_to_BCD(month));
  write(write_year, Decimal_to_BCD(year));
}
void loop(){}




// int main(void){

//   //Write "Hello World!" on the LCD screen
//   //UARTCharPut(UART7_BASE, ser_lcd_command);

//   //Clear Display
//   clearDisplay();   
//   putPhrase("Hello World!");
//   while(1){
//     if (UARTCharsAvail(UART1_BASE))
//     {
//      UARTCharPut(UART1_BASE, UARTCharGet(UART1_BASE));
//    }
//  }
// }

// void putChar(char character){
//   //Place character
//   UARTCharPut(b1, character);
// }

// void putPhrase(char *string){
//   int i;
//   for(i = 0; i < 12; i++){
//     putChar(string[i]);
//   }
// }

// void clearDisplay(){
//   UARTCharPut(b1, 0xFE);
//   UARTCharPut(b1, 0x01);
// }


// void selectLineOne(){
//   UARTCharPut(b1, 0xFE);
//   UARTCharPut(b1, 0x80);
// }

// void selectLineTwo(){
//   UARTCharPut(b1, 0xFE);
//   UARTCharPut(b1, 0xC0);
// }

// void changeCursorUnderscore(){
//   UARTCharPut(b1, 0xFE);
//   UARTCharPut(b1, 0x0E);
// }

