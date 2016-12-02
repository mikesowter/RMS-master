/*************************************************************
project:		RMS
author:			M Sowter
description:	Calculates power, Irms, Vrms from sample data

Error sources:	V - 0.3%
								I - 0.6%			noise of 1mVp = 0.01Arms
								ADC - 0.2%		bandwidth < 2kHz

Resolution:		V - 0.3V
							I - 0.025A
************************************************************
#include <math.h>
#include <Print.h>

extern volatile int buffer[8][200];			// filled under interrupt from ADCH register
extern volatile int bufferNum;				// selects buffer and analog mux (0-7)
extern volatile int bufferPtr ;				// Offset in current buffer

extern int nums;							// number of samples per power cycle
extern int power[7];						// Sum of sampled V*I
extern int Irms[7];							// Sum of sampled I*I
extern int Vrms;							// Sum of sampled V*V


float powerSum;
float IrmsSum;
float VrmsSum;
float volts,amps;
float Voff,Ioff;

int circuit;

void calcValues() {

	for (circuit=0;circuit<7;circuit++)
    {
		powerSum=0.0;
		IrmsSum=0.0;
		VrmsSum=0.0;

		printf("\n\rcct:%i",circuit);

		for (int i=0;i<nums;i++)	// first calculate offset in the channels
		{
			Voff += buffer[0][i];
			Ioff += buffer[circuit+1][i];
//			printf(",%i",buffer[circuit + 1][i]);
		}
		Voff /= (float)nums;
		Ioff /= (float)nums;

		for (int i=0;i<nums;i++)	// then using that offset, scale the channels
		{
			volts = (float)(buffer[0][i]-Voff);
			amps = (float)(buffer[circuit+1][i]-Ioff)/10.0;
			if (Ioff<500) amps=0.0;	// no circuit connected
			powerSum += volts * amps;
			IrmsSum += amps * amps;
			VrmsSum += volts * volts;
		}
		power[circuit] = powerSum/(float)nums;
		Irms[circuit] = 100.0*sqrt(IrmsSum / (float)nums);	// scale to 30.00A
	}
	Vrms = 10.0*sqrt(VrmsSum / nums);						// scale to 240.0V
}
*/
