#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/can.h"


void setup(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN1);
	SysCtlPeripheralEnable(GPIO_PORTF_BASE);
	SysCtlPeripheralEnable(GPIO_PORTA_BASE);
	GPIOPinTypeCAN(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_3);
	GPIOPinTypeCAN(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	GPIOPinConfigure(GPIO_PF0_CANRX);
	GPIOPinConfigure(GPIO_PF3_CANFX);

	GPIOPinConfigure(GPIO_PA0_CANRX);

	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	tCANBitClkParms CANBitClk;
	tCANMsgObject sMsgObjectRx;
	tCANMsgObject sMsgObjectTx;


	uint8_t pui8BufferIn[8];
	uint8_t pui8BufferOut[8];


	CANInit(CAN0_BASE);
	CANInit(CAN1_BASE);

	CANBitTimingSet(CAN1_BASE, &CANBitClk);

	CANEnable(CAN0_BASE);
	CANEnable(CAN1_BASE);

	sMsgObjectRx.ui32MsgID=(0x400);
	sMsgObjectRx.ui32MsgIDMask = 0x7f8;
	sMsgObjectRx.ui32Flags = MSG_OBJ_USE_ID_FILTER | MSG_OBJ_FIFO;

	CANMessageSet(CAN0_BASE, 1, &sMsgObjectRx, MSG_OBJ_TYPE_RX);
	CANMessageSet(CAN0_BASE, 2, &sMsgObjectRx, MSG_OBJ_TYPE_RX);
	CANMessageSet(CAN0_BASE, 3, &sMsgObjectRx, MSG_OBJ_TYPE_RX);

	sMsgObjectRx.ui32Flags = MSG_OBJ_USE_ID_FILTER;
	CANMessageSet(CAN0_BASE, 4, &sMsgObjectRx, MSG_OBJ_TYPE_RX);

	sMsgObjectTx.ui32MsgID = 0x400;
	sMsgObjectTx.ui32Flags = 0;
	sMsgObjectTx.ui32MsgLen = 8;
	sMsgObjectTx.pui8MsgData = pui8BufferOut;
	CANMessageSet(CAN0_BASE, 2, &sMsgObjectTx, MSG_OBJ_TYPE_TX);
    //
    // Wait for new data to become available.
    //
	while((CANStatusGet(CAN1_BASE, CAN_STS_NEWDAT) & 1) == 0)
	{
        //
        // Read the message out of the message object.
        //
		CANMessageGet(CAN1_BASE, 1, &sMsgObjectRx, true);
	}
}

void loop(){}
