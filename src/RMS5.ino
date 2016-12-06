#include <Arduino.h>

/*************************************************************
project: 		RMS power meter
version:		5.0
author: 		M Sowter
description:
Reads voltage and current in an AC distribution board and extracts RMS power, Power Factor and VARS from up to 15
circuits, transmitting these values to external 32k SPI RAM after each reading cycle.
A free running ADC populates up to 16 circular buffers, the first representing voltage and the others current.

First the voltage is read, and the number of samples per cycle determined, varying from 185-200 with freq 48-52 Hz.
The current capture begins with the positive transition of the voltage waveform. The current buffer index is reset to begin
the storage of one full power cycle. On reaching 200 samples, the ADC mux is moved to the next analog channel and the
storage to the next buffer, awaiting the beginning of the next cycle.

Captures are always triggered by the positive transition of the voltage waveform, and so total capture time requires up to
32 cycles or 640ms at 50Hz. Waveforms are assumed to be stable over the sampling period.

Role of external processor:
The external processor (ESP family) is responsible for WiFi and web access, together with NTP and FTP processes.
The RAM values are squared and summed to give RMS voltage and current values. Power is calc'd as the sum
of V*I for each circuit divided by the number of samples collected, then power factor by power/(Vrms*Irms).
The data is time tagged and then stored for further averaging processes.
All calcs are performed in 16 bit precision and then configured for display on a WEB page.

*************************************************************/
#include <RMS5.h>

void setup() {

	pinMode(acInPin, INPUT);
	pinMode(acOutPin, OUTPUT);
	pinMode(capturePin, OUTPUT);
	pinMode(ledPin, OUTPUT);

	Serial.begin(115200);
	printf_begin();
	printf("\n\rRMS Version 5   2016-12-04\n\n\r");

	radioSetup();

	initADC();
	bufferNum = -1;
	bufferPtr = 0;
}

void loop()
{
	digitalWrite(ledPin, LOW);
	waitForXing();
	sbi(ADCSRA, ADEN);
	sbi(ADCSRA, ADIE);
	sbi(ADCSRA, ADSC);

	if (bufferPtr==nums) {
		// Disable ADC and stop Free Running Conversion Mode
		cbi(ADCSRA, ADEN);
		cbi(ADCSRA, ADIE);
		cbi(ADCSRA, ADSC);
		// debugging
		// printf("%x ",ADMUX);
		// Increment mux to next analog
		bufferNum++;
		bufferPtr = 0;
		ADMUX &= 0xF0;
		ADMUX |= (bufferNum & 0x0F);
		// throw away last value
		value = ADCL;
		value += 256 * ADCH;
		if (bufferNum == numChannels) {
			bufferFull = true;
			digitalWrite(ledPin, HIGH);
		}
	}
	if (bufferFull) {

/*	  for (bufferNum=0;bufferNum < numChannels;bufferNum++) {
			Serial.println(bufferNum);
      for (int j = 0; j<40; j++) {
        Serial.print(buffer[bufferNum][j]);
        Serial.print(',');
      }
    Serial.println();
	}		*/
		ADMUX &= 0xF0;
		bufferNum = 0;
		bufferFull = false;
		bufferPtr = 0;
		digitalWrite(ledPin, LOW);
		// and while debugging
		// delay(1000);
		calcValues();
		// Serial.print("whole cycle takes: ");
		// Serial.println(millis()-before);
		// before=millis();
		sendValues();
	}
	// reenable ADC Free Running Conversion Mode
	waitForXing();
	sbi(ADCSRA, ADEN);
	sbi(ADCSRA, ADIE);
	sbi(ADCSRA, ADSC);
}

//-----------------------------------------------------------------------------
// ADC Conversion Complete Interrupt
//-----------------------------------------------------------------------------
ISR(ADC_vect) {
	// When ADCL is read, the ADC Data Register is not updated until ADCH
	// is read.
	value = ADCL;
	value += 256 * ADCH;
	if (bufferPtr < nums)	{
		buffer[bufferNum][bufferPtr++] = value;
	}
}

void waitForXing() {
	digitalWrite(acOutPin,1);
	while (digitalRead(acInPin)==0) { }  //wait for positive voltage Xing
  while (digitalRead(acInPin)==1) { }  //wait till voltage drops below 0
	digitalWrite(acOutPin,0);
	while (digitalRead(acInPin)==0) { }  //wait for positive voltage Xing
}
