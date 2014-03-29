#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

uint32_t ui32ADC0Value[8];
volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;
uint32_t ui32Value;

void setup() {

	Serial.begin(9600);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_3);


	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH0 | ADC_CTL_IE |ADC_CTL_END);
		ADCSequenceEnable(ADC0_BASE,0);


	Serial.print("initializing");
	
}

// the loop routine runs over and over again forever:
void loop() {
				ADCProcessorTrigger(ADC0_BASE, 0);


	Serial.print("Looping");






// 	ADCSequenceStepConfigure(ADC0_BASE, 0,0, ADC_CTL_TS |ADC_CTL_IE | ADC_CTL_END );

	Serial.print("Configuring Done");


	while(!ADCIntStatus(ADC0_BASE, 0 , false)){}
		ADCSequenceDataGet(ADC0_BASE,0, ui32ADC0Value);
	uint32_t sum = 0;
	for(int i = 0; i < 8; i ++)
	{
		sum += ui32ADC0Value[i];
	}

	ui32TempAvg= sum/8;
	ui32TempAvg = ui32TempAvg * 3300/4096;

	int value = (ui32TempAvg - 500)/10;

	
	Serial.println(value);

  delay(1000); // delay in between reads for stability

}