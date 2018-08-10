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

void calcValues() {
	uint64_t ADCoffset = 0;
	for (uint8_t i=0;i<numSamples;i++) {	     // then using that offset, scale the channels
		ADCoffset += buffer[1][i];
	}
	Voff = (float)(ADCoffset/numSamples);

	for (uint8_t circuit=1;circuit<numChannels;circuit++) {
		powerSum=0.0;
		IrmsSum=0.0;
		VrmsSum=0.0;

		for (uint8_t i=0;i<numSamples;i++) {   			// then using that offset, scale the channels
			volts = 1.0*((float)buffer[0][i]-Voff);        	// see calibration spreadsheet
			amps = 0.1*((float)buffer[circuit][i]-Voff);   	// see calibration spreadsheet
			powerSum += (volts * amps);
			IrmsSum  += (amps * amps);
			VrmsSum  += (volts * volts);
		}
		power[circuit] = powerSum / (float)numSamples;
		Irms[circuit] = sqrt(IrmsSum / (float)numSamples);
	}
	Vrms = sqrt(VrmsSum / (float)numSamples);
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
