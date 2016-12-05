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

extern const uint8_t numChannels;   // number of analogs (V+I)
extern const uint8_t nums;			// number of samples per power cycle
extern volatile uint16_t buffer[numChannels][200];	// filled under interrupt from ADCH register
extern volatile uint8_t bufferPtr;				          // Offset in current buffer
extern uint8_t bufferNum;				          // selects buffer and analog mux (0-7)

float power[7];					        // Sum of sampled V*I
float Irms[7];					       	// Sum of sampled I*I
float Vrms;						         	// Sum of sampled V*V
float powerSum;
float IrmsSum;
float VrmsSum;
float volts,amps;

uint8_t circuit;

void calcValues() {

	for (circuit=1;circuit<numChannels;circuit++) {
		powerSum=0.0;
		IrmsSum=0.0;
		VrmsSum=0.0;

		for (uint8_t i=0;i<nums;i++)	     // then using that offset, scale the channels
		{
			volts = (float)(buffer[0][i]-512)/1.0;          // volts to be scaled
			amps = (float)(buffer[circuit][i]-512)/10.0;    // needs to be scaled separately each channel
			powerSum += volts * amps;
			IrmsSum  += amps * amps;
			VrmsSum  += volts * volts;
		}
		power[circuit] = powerSum/(float)nums;
		int PL=100*power[circuit]/100;
		int PR=(int)(100*power[circuit])%100;
		Irms[circuit] = 100.0*sqrt(IrmsSum / (float)nums);
		printf("\r\nCircuit: %i  Power: %i.%i",circuit,PL,PR);
	}
	Vrms = sqrt(VrmsSum / nums);
	printf("\r\nVolts:  %i.%i",(int)Vrms,(int)(100*Vrms)%100);
}
