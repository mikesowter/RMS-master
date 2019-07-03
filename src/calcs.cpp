/*************************************************************
project:		RMS
author:			M Sowter
description:	Calculates power, Irms, Vrms from sample data

Error sources:	V - 0.3%
								I - 0.6%		noise of 1mVp = 0.01Arms
								ADC - 0.2%		bandwidth < 2kHz

Resolution:		V - 0.3V, Vp = 1.0V
							I - 0.025A
************************************************************/
#include "extern.h"

char* f2s2(float f);
char* f2s4(float f);

void calcValues() {
	float ADCoffset = 0.0;

	// first the voltage on A0-A2

	VrmsSum = 0.0;
	for (uint8_t i=0;i<numSamples;i++) {	     // calculate average voltage offset
		ADCoffset += Vsmooth[0][i];
	}
	Voff = (ADCoffset/(float)numSamples);
	for (uint8_t i=0;i<numSamples;i++) {   			// then using that offset, scale the channels
		volts = vScale*(Vsmooth[0][i]-Voff);    
		VrmsSum  += (volts * volts);
	}
	Vrms = (float) sqrt(VrmsSum / (float)numSamples);
	Vmin = vScale*(min(Vsmooth[1][2],Vsmooth[0][40])-Voff);
  Vmax = vScale*(max(Vsmooth[2][2],Vsmooth[0][136])-Voff);
	
	Serial.println();
	Serial.print("Vrms = ");
	Serial.print(Vrms);
	Serial.print(", Vmin = ");
	Serial.print(Vmin);
	Serial.print(", Vmax = ");
	Serial.print(Vmax);
	Serial.print(", Freq = ");
	Serial.println(getFreq());

	// then the currents on A3,A4...

	ADCoffset = 0.0;
	for (uint8_t i=0 ; i<numSamples ; i++) ADCoffset += (float)Ismooth[0][i];
	Ioff = ADCoffset/(float)numSamples;

	for (uint8_t circuit=1 ; circuit<=NUM_CHANNELS ; circuit++) {
		powerSum = 0.0;
		IrmsSum = 0.0;
		Imax = -10.0;
		Imin = +10.0;
	
		for (uint8_t i=0 ; i<numSamples ; i++) {   			// subtract that offset & scale the channels
			volts = vScale*(Vsmooth[0][i]-Voff);
			amps = iScale*((float)Ismooth[circuit-1][i]-Ioff);
			if (amps > Imax) Imax = amps;
			if (amps < Imin) Imin = amps;
			powerSum += (volts * amps);
			IrmsSum  += (amps * amps);
		}

		Wrms[circuit] = 0.9*Wrms[circuit] + 0.1*abs(powerSum / (float)numSamples);
		Irms[circuit] = (float) sqrt(IrmsSum / (float)numSamples);
		Serial.print("Circuit[");
		Serial.print(circuit);
		Serial.print("]: Imin = ");
		Serial.print(Imin);
		Serial.print(", Imax = ");
		Serial.print(Imax);
		Serial.print(", Irms = ");
		Serial.print(Irms[circuit]);
		Serial.print(", Wrms = ");
		Serial.println(Wrms[circuit]);
	}
}

// convert float into char string ddddd.dd
char* f2s2(float f) {
  dtostrf((double)f, 0, 2, fltStr);
  return fltStr;
}

// convert float into char string ddddd.dddd
char* f2s4(float f) {
  dtostrf((double)f, 0, 4, fltStr);
  return fltStr;
}

void printBuffers() {
  for (int i=0;i<3;i++) {
		Serial.println(i);
		for (int j = 0; j<192; j++) {
			Serial.print(Vsmooth[i][j]);
			Serial.print(',');
		}
    Serial.println();
		delay(1);
	}
}

float getFreq() {
	waitForXing();
	uint32_t t = micros();
	for (int i=0;i<10;i++) {
		waitForXing();
	}
	//numSamples = (192.0/AVR_CLK_ERR)*(float)t1/200.0;  // AVR_CLK_ERR is local ms clock error
	return 10000000.0/(float)(micros()-t);
}
