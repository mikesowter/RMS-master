/*************************************************************
project:		RMS
author:			M Sowter
description:	Calculates power, Irms, Vrms from sample data

Error sources:	V - 0.3%
								I - 0.6%			noise of 1mVp = 0.01Arms
								ADC - 0.2%		bandwidth < 2kHz

Resolution:		V - 0.3V
							I - 0.025A
************************************************************/
#include <math.h>
#include <Print.h>

extern const int numChannels;
extern volatile uint16_t buffer[numChannels][200];	// filled under interrupt from ADCH register
extern volatile uint8_t bufferNum;				// selects buffer and analog mux (0-7)
extern volatile uint8_t bufferPtr;				// Offset in current buffer

extern const uint8_t nums;						// number of samples per power cycle
float power[7];					        // Sum of sampled V*I
float Irms[7];					       	// Sum of sampled I*I
float Vrms;						         	// Sum of sampled V*V

float powerSum;
float IrmsSum;
float VrmsSum;
float volts,amps;
float Voff,Ioff;

uint8_t circuit;

void calcValues() {

	for (circuit=1;circuit<numChannels;circuit++) {
		powerSum=0.0;
		IrmsSum=0.0;
		VrmsSum=0.0;

		Serial.println(circuit);

		for (uint8_t i=0;i<nums;i++)	// first calculate offset in the channels
		{
			Voff += (float)buffer[0][i];
			Ioff += (float)buffer[circuit][i];
//			printf(",%i",buffer[circuit + 1][i]);
		}
		Voff /= (float)nums;
		Ioff /= (float)nums;

		for (uint8_t i=0;i<nums;i++)	     // then using that offset, scale the channels
		{
			volts = (float)(buffer[0][i]-Voff)/2.0;          // volts to be scaled
			amps = (float)(buffer[circuit][i]-Ioff)/10.0;  //possibly needs to be scaled separately each channel
			if (Ioff<500) amps=0.0;	// no circuit connected
			powerSum += volts * amps;
			IrmsSum  += amps * amps;
			VrmsSum  += volts * volts;
		}
		power[circuit] = powerSum/(float)nums;
		Irms[circuit] = 100.0*sqrt(IrmsSum / (float)nums);
	}
	Vrms = 10.0*sqrt(VrmsSum / nums);
}
