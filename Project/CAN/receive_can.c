#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/can.h"


void setup(){
	Serial.begin(9600);
	Serial.println("DEBUG1");
	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
	SysCtlPeripheralEnable(GPIO_PORTE_BASE);
	GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	GPIOPinConfigure(GPIO_PE4_CAN0RX);
	GPIOPinConfigure(GPIO_PE5_CAN0TX);

	tCANMsgObject sCANMessage;
    uint8_t ucMsgData[8];

			Serial.println("DEBUG2");

	uint8_t pui8BufferIn[8];
	uint8_t pui8BufferOut[8];

			Serial.println("DEBUG3");

	CANInit(CAN0_BASE);

		    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);

	CANEnable(CAN0_BASE);

  sCANMessage.ui32MsgID = 0;                        // CAN msg ID - 0 for any
    sCANMessage.ui32MsgIDMask = 0;                    // mask is 0 for any ID
    sCANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    sCANMessage.ui32MsgLen = 8;                       // allow up to 8 bytes

    //
    // Now load the message object into the CAN peripheral.  Once loaded the
    // CAN will receive any message on the bus, and an interrupt will occur.
    // Use message object 1 for receiving messages (this is not the same as
    // the CAN ID which can be any value in this example).
    //
    CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_RX);

			Serial.println("DEBUG5");

	while((CANStatusGet(CAN0_BASE, CAN_STS_NEWDAT) & 1) == 0)
	{
        //
        // Read the message out of the message object.
        //
		CANMessageGet(CAN0_BASE, 1, &sCANMessage, true);
			Serial.println(sCANMessage.ui32MsgLen);

	}
				Serial.println(sCANMessage.ui32MsgLen);

			Serial.println(sCANMessage.ui32MsgID);

}

void loop(){}
