// Defines for setting and clearing register bits

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#include <SPI.h>
#include <math.h>

void calcValues();
void sendValues();
void initADC();
void setupSPI();
void testSPI();


// set constants:
const uint8_t nums=193;							// number of samples per 20ms power cycle
const uint8_t numChannels = 3;    	// the total number of analogs channels (V and Is)
const uint8_t acInPin = 2;    			// the pin connected to a squared 50Hz waveform
const uint8_t acOutPin = 3;    			// the pin displaying 50Hz activity
const uint8_t capturePin = 5;				// displaying capture period
const uint8_t CEPin = 9;            // radio enable
const uint8_t CSNPin = 10;          // radio enable
const uint8_t ledPin = 13;      		// the pin of the onboard LED

// set variables:
uint8_t bufferNum;					        // buffer[0] is voltage, buffers[1->] are current
volatile uint8_t bufferPtr;				  // Offset in current buffer
volatile uint16_t value;						// most recent ADC value
volatile uint16_t buffer[numChannels][200];			// Storage for all raw counts from ADC
bool bufferFull = false;
long before = 0;
uint16_t buff[16];                  // 32 byte data buffer
uint16_t packetNum=0x1000;				  // packet sequence identifier
