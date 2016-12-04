// Defines for setting and clearing register bits

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#include <SPI.h>

void calcValues();
void sendValues();
void initADC();

// set constants:
const int numChannels = 7;    			// the total number of analogs channels (Vs and Is)
const uint8_t acInPin = 2;    			// the pin connected to a squared 50Hz waveform
const uint8_t acOutPin = 3;    			// the pin displaying 50Hz activity
const uint8_t capturePin = 5;				// displaying capture period
const uint8_t ledPin = 13;      		// the pin of the onboard LED

// set variables:
const uint8_t nums=193;							      // number of samples per power cycle

volatile uint8_t bufferNum;					// buffer[0] is voltage, buffers[1-7] are current
volatile uint8_t bufferPtr;				// Offset in current buffer
volatile uint16_t value;						// most recent ADC value
volatile bool bufferFull = false;
volatile uint16_t buffer[numChannels][200];			// Storage for all raw counts from ADC
