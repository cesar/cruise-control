#include <stdint.h> 

#include <stdbool.h> 

#include "inc/hw_memmap.h" 

#include "inc/hw_types.h" 

#include "driverlib/sysctl.h" 

#include "driverlib/gpio.h" 

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

int lcd_mode = 0; //0 for initialization and 1 for writing

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

int main() {

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);     

  GPIOPinTypeGPIOOutput(port_A, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4); 
  GPIOPinTypeGPIOOutput(port_C, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); 
  GPIOPinTypeGPIOOutput(port_D, GPIO_PIN_6); 
  GPIOPinTypeGPIOOutput(port_E, GPIO_PIN_0); 
  GPIOPinTypeGPIOOutput(port_F, GPIO_PIN_4); 
  
  initializeDisplay();
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

void toggle_mode()
{
  if(lcd_mode == 0)
  {

      GPIOPinWrite(port_C, GPIO_PIN_5, 0x0);

  }

  else{
      GPIOPinWrite(port_C, GPIO_PIN_5, pin_5);


  }
}

void initializeDisplay() {

  //Wait more than 20ms after on sequence.

  //RS low
  GPIOPinWrite(port_C, GPIO_PIN_5, 0x0);

  
  //Function Set
  writeDataPins(0,0,1,1,0,0,0,0);

  
  //CLear Screen
  writeDataPins(0,0,0,0,0,0,0,1);
  
  //Display on/off control
  writeDataPins(0,0,0,0,1,1,1,0);
  

  //Entry mode set
  writeDataPins(0,0,0,0,0,1,1,0);
  
  //Start to write move RS to High
  GPIOPinWrite(port_C, GPIO_PIN_5, pin_5);

  //Writes an H
  writeDataPins(0,1,0,0,1,0,0,0);
  
  //Writes an I
  writeDataPins(0,1,0,0,1,0,0,1);

  //Moves the curso to the right
  writeDataPins(0,0,0,1,0,1,0,0);

  //Writes a J
  writeDataPins(0,1,0,1,1,1,0,1);
  
  writeDataPins(0,1,0,1,1,1,0,1);




}

