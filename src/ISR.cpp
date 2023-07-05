#include "extern.h"

//-----------------------------------------------------------------------------
// ADC Conversion Complete Interrupt
//-----------------------------------------------------------------------------
ISR(ADC_vect) {
	value = ADCL;
	value += 256 * ADCH;
	if (bufferPtr < numSamples)	{
		ADCbuf[bufferPtr] = ADCbuf[bufferPtr] + value;
		bufferPtr++;
	}
	else {
		cbi(ADCSRA, ADEN);
		cbi(ADCSRA, ADIE);
		cbi(ADCSRA, ADSC);
	}
}

void waitForXing() {
  	pwrOutage = false;
	t0 = millis();
  	while (digitalRead(AC_IN_PIN) == 1) {		// wait for negative going Xing
 		if (millis()-t0 > 11) {
			pwrOutage = true;
			return;					// return if no voltage input
		}
	}  
	t0 = millis();
	while (digitalRead(AC_IN_PIN) == 0) {		// wait for positive going Xing
 		if (millis()-t0 > 11) {
			pwrOutage = true;
			return;					// return if no voltage input
		}
	}  
}

