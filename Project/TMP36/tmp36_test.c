#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

uint32_t voltage_samples[8];

void setup_tmp36(){
	//Enable the ADC Module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	//Enable ADC Input on Pin E3
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_3);

	//Configure the step sequence for higher temperature precision
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH0 );
	ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH0 | ADC_CTL_IE |ADC_CTL_END);
	
	//Enable the ADC Sequencer	
	ADCSequenceEnable(ADC0_BASE,0);

}

int get_analog_temp()
{	
	//Triggers the processor to start a voltage sequence
	ADCProcessorTrigger(ADC0_BASE, 0);

	//Wait for ADC to be available
	while(!ADCIntStatus(ADC0_BASE, 0 , false)){}
	
	ADCSequenceDataGet(ADC0_BASE,0, voltage_samples);

	//Add up the samples
	uint32_t sum = 0;
	for(int i = 0; i < 8; i ++)
	{
		sum += voltage_samples[i];
	}
	//Take out the average and scale it up by 3.3V and divide by the resolution of 12 bits
	avg_temp= sum/8 * 3300/4096;



	 int temp_celcius = (avg_temp - 500)/10;
	 return temp_celcius;


}

void disable_module()
{
	ADCSequenceDisable(ADC0_BASE,0);


}

void enable_module()
{
	ADCSequenceEnable(ADC0_BASE,0);


}



void setup()
{
	Serial.begin(9600);
	setup_tmp36();
}

void loop()
{

	int temp = get_analog_temp();
	Serial.println(temp);
	delay(1000);

}