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

	// first the voltage on A0

	VrmsSum = 0.0;
	for (uint8_t i=0;i<numSamples;i++) {	     // calculate average voltage offset
		ADCoffset += smooth[0][i];
	}
	Voff = (ADCoffset/(float)numSamples);
	for (uint8_t i=0;i<numSamples;i++) {   			// then using that offset, scale the channels
		volts = vScale*(smooth[0][i]-Voff);    
		VrmsSum  += (volts * volts);
	}
	Vrms = (float) sqrt(VrmsSum / (float)numSamples);
	Vmax = vpScale*(smooth[NUM_CHANNELS-2][2]-Voff+30);
	Vmin = vpScale*(smooth[NUM_CHANNELS-1][2]-Voff);

	Serial.println();
	Serial.print("Vrms = ");
	Serial.print(Vrms);
	Serial.print(", Vmin = ");
	Serial.print(Vmin);
	Serial.print(", Vmax = ");
	Serial.print(Vmax);
	Serial.print(", Freq = ");
	Serial.println(getFreq());

	// then the currents on a1,a2...

	for (uint8_t circuit=1;circuit<5;circuit++) {
		powerSum = 0.0;
		IrmsSum = 0.0;
		Imax = -1.0;
		Imin = +1.0;
		ADCoffset = 0.0;
		for (uint8_t i=0;i<numSamples;i++) {
			ADCoffset += smooth[circuit][i];
		}
		Ioff = (ADCoffset/(float)numSamples);
		for (uint8_t i=0;i<numSamples;i++) {   			// subtract that offset & scale the channels
			volts = vScale*(smooth[0][i]-Voff);
			amps = iScale*(smooth[circuit][i]-Ioff);
			if (amps > Imax) Imax = amps;
			if (amps < Imin) Imin = amps;
			powerSum += (volts * amps);
			IrmsSum  += (amps * amps);
		}
		Wrms[circuit] = abs(powerSum / (float)numSamples);
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
  for (int i=0;i<1;i++) {
		Serial.println(i);
		for (int j = 0; j<192; j++) {
			Serial.print(smooth[i][j]);
			Serial.print(',');
		}
    Serial.println();
		delay(1);
	}
}

float getFreq() {
	waitForXing();
	t0 = micros();
	for (int i=0;i<10;i++) {
		waitForXing();
	}
	//numSamples = (192.0/AVR_CLK_ERR)*(float)t1/200.0;  //where .993 is local ms clock error
	return 10000000.0/(float)(micros()-t0);
}
