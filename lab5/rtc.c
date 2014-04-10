#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/ssi.h"


uint32_t data;
uint32_t response;
uint32_t garbage;


void select()
{
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x2);


}
void deselect()
{
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x0);


}


void setup()
{ 
  data = 0x27;
  SysCtlClockSet(SYSCTL_SYSDIV_5| SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); 

  //Run the clock at 40MHz
  Serial.begin(9600);
  Serial.println("Starting");

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

  SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, 500000, 8 );
  Serial.println("Clock configured.");

  //Enable SSI
  SSIEnable(SSI3_BASE);
  Serial.println("SSI Enabled");


    //CS High - Select
  select();

  //Send write command
  SSIDataPut(SSI3_BASE,0x8F);

  //Send write data argument to registers
  SSIDataPut(SSI3_BASE,0x00);

     while(SSIBusy(SSI3_BASE));



  //CS Low - Deselect
  deselect();

  
  
 
}
void loop(){}


