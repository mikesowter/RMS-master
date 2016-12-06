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

extern const uint8_t numChannels;   			// number of analogs (V+I)
extern const uint8_t nums;								// number of samples per power cycle
extern volatile uint16_t buffer[numChannels][200];	// filled under interrupt from ADCH register
extern volatile uint8_t bufferPtr;				          // Offset in current buffer
extern uint8_t bufferNum;				          // selects buffer and analog mux (0-7)

float power[numChannels];					        // Sum of sampled V*I
float Irms[numChannels];					       	// Sum of sampled I*I
float Vrms,Voff;						         			// Sum of sampled V*V
float powerSum;
float IrmsSum;
float VrmsSum;
float volts,amps;

void calcValues() {
	uint64_t ADCoffset = 0;
	for (uint8_t i=0;i<nums;i++) {	     // then using that offset, scale the channels
		ADCoffset += buffer[1][i];
	}
	Voff = (float)(ADCoffset/nums);

	for (uint8_t circuit=1;circuit<numChannels;circuit++) {
		powerSum=0.0;
		IrmsSum=0.0;
		VrmsSum=0.0;

		for (uint8_t i=0;i<nums;i++) {   // then using that offset, scale the channels
			volts = 0.884*((float)buffer[0][i]-Voff);        // see calibration spreadsheet
			amps = 0.0492*((float)buffer[circuit][i]-Voff);    // see calibration spreadsheet
			powerSum += (volts * amps);
			IrmsSum  += (amps * amps);
			VrmsSum  += (volts * volts);
		}
		power[circuit] = powerSum / (float)nums;
		Irms[circuit] = sqrt(IrmsSum / (float)nums);
	}
	Vrms = sqrt(VrmsSum / (float)nums);
	Serial.print("Vrms = ");
	Serial.print((int)Vrms);
	Serial.print(".");
	Serial.println((int)(10.0*Vrms)-10*(int)Vrms);
	float p = power[1];
	Serial.print("Power = ");
	Serial.print((int)p);
	Serial.print(".");
	Serial.println((int)(10.0*p)-10*(int)p);
	//delay(1000);
}
