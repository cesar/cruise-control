#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/can.h"


int main(void){
		// Serial.begin(9600);
			// Serial.println("DEBUG1");

	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
	SysCtlPeripheralEnable(GPIO_PORTE_BASE);
	GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	GPIOPinConfigure(GPIO_PE4_CAN0RX);
	GPIOPinConfigure(GPIO_PE5_CAN0TX);
		// Serial.println("DEBUG2");

tCANMsgObject sCANMessage;
    uint8_t ucMsgData[4];

	

	// Serial.println("DEBUG4");
	  sCANMessage.ui32MsgID = 1;                        // CAN message ID - use 1
    sCANMessage.ui32MsgIDMask = 0;                    // no mask needed for TX
    sCANMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;    // enable interrupt on TX
    sCANMessage.ui32MsgLen = sizeof(ucMsgData);       // size of message is 4
    sCANMessage.pui8MsgData = ucMsgData;

	CANInit(CAN0_BASE);
	    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);



	CANEnable(CAN0_BASE);
	
	for(;;){
	CANMessageSet(CAN0_BASE, 2, &sCANMessage, MSG_OBJ_TYPE_TX);
  		// Serial.println("DEBUG6");
  		// delay(1000);
  	}

}

void loop(){}
