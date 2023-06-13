/*************************************************************
project: 		RMS power meter
version:		5.1
author: 		M Sowter
description:
Reads voltage and current in an AC distribution board and extracts RMS power, Power Factor and VARS from up to 15
circuits, transmitting these values to a slave esp8266 after each complete cycle.
A free running ADC populates up to 16 circular buffers, the first representing voltage and the others current.

The voltage waveform is read, and the number of samples per cycle determined, varying from 185-200 with freq 48-52 Hz.
The current captures begin with the positive transition of the voltage waveform. The current buffer index is reset to begin
the storage of one full power cycle. On reaching the appropriate number of samples, the ADC mux is moved to the next analog channel and the storage to the next buffer, awaiting the beginning of the next voltage cycle.

Captures are always triggered by the positive transition of the voltage waveform, and so total capture time requires up to
32 cycles (for 15 circuits) or 640ms at 50Hz. Voltage waveforms are assumed to be stable over the sampling period. The values are squared and summed to give RMS voltage and current values. Power is calc'd as the sum of V*I for each circuit divided by the number of samples collected, then power factor by power/(Vrms*Irms).

Role of external slave processor:
The external processor (ESP family) is responsible for WiFi and web access, for NTP and metrics scrape processes.
The data is time tagged and then stored for further averaging processes. All calcs are performed in 16 bit precision and then configured for capture by a TBDB like Prometheus.

*************************************************************/
#include "main.h"

void setup() {

	pinMode(AC_IN_PIN, INPUT);
	pinMode(RESET_PIN, OUTPUT);
	pinMode(RED_PIN, OUTPUT);
	pinMode(GRN_PIN, OUTPUT);
	pinMode(BLU_PIN, OUTPUT);
	digitalWrite(RESET_PIN, 0);		// reset slave
	delay(10);
	digitalWrite(RESET_PIN, 1);

	Serial.begin(115200);
	Serial.println("\n\rRMS Version 6.2  2023-06-08");

	initADC();
//	getFreq();		//local clock running slow at .99936?
	setBlue();
	setupSPI();
	getSlaveTime();
	SPI.end();
	Serial.print(dateStamp());
	Serial.print(" ");
	Serial.println(timeStamp());
	osCount = 0;
	bufferNum = 0;
	bufferPtr = 0;
	loopStart = millis();
	allOff();
}

void loop() {
	if (bufferPtr == 0) {
		scanStart = millis();
		waitForXing();
		sbi(ADCSRA, ADEN);
		sbi(ADCSRA, ADIE);
		sbi(ADCSRA, ADSC);
		delay(22);		  // minimal activity while sampling
	}
	if (bufferPtr >= numSamples) {  	//should always be true
		// Increment oversample count
		osCount++;
		bufferPtr = 0;
		// always throw away last value
		value = ADCL;
		value += 256 * ADCH;
		// if we have enough sample runs just average them
		if (osCount >= OVER_SAMPLE) {
			if (bufferNum < 3) {			// bufferNum 0-2 are voltage
				for (int i = 0 ; i<numSamples ; i++) {
					Vsmooth[bufferNum][i] = (float)ADCbuf[i]/(float)OVER_SAMPLE;
					ADCbuf[i] = 0;
				}
			}
			else {										// bufferNum 3-10 are current
				for (int i = 0 ; i<numSamples ; i++) {
					Ismooth[bufferNum-3][i] = (ADCbuf[i] + OVER_SAMPLE/2)/OVER_SAMPLE;
					ADCbuf[i] = 0;
				}
			}
			osCount = 0;
			// Increment mux to next analog
			bufferNum++;
			ADMUX &= 0xF0;
			ADMUX |= (bufferNum & 0x07);
			if ( bufferNum < 8 ) cbi(ADCSRB,3);  //mux5 (ADCSRB b3) is used as MUX bit 3 
			else sbi(ADCSRB,3);
			// check for full buffers
			if (bufferNum >= NUM_CIRCUITS+3) {
				t1 = millis() - scanStart;
//				printBuffers();
//				delay(100);
				calcStart = millis();
				calcValues();			
				t2 = millis() - calcStart;
				setupSPI();				
				// load data to send to slave
				loadValues();
				send(SPIbuf[0]);
				SPI.end();
				t3 = millis() - calcStart - t2;
				// setup for new scan every 2.5 seconds
				ADMUX &= 0xF0;
				cbi(ADCSRB,3); 
				bufferNum = 0;
				t4 = millis()-loopStart;
	//			Serial.print("  loop time: ");
	//			Serial.println(t4);
				setGreen();
				if (t4 < 1000) delay(1000 - t4);
	//			allOff();
				loopStart = millis();
			} //end full buffers
		} //end oversampling
	} //end full ADCbuf
	else {
		setRed();
		Serial.println("failed to sample");
		delay(2000);
	}
} //end loop

