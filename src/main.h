#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#include <SPI.h>
#include <math.h>
#include <Time.h>
#include <arduino.h>

void calcValues();
void loadValues();
void load2Bytes();
void initADC();
void setupSPI();
void getSlaveTime();
void waitForXing();
void printBuffers();
void send(uint8_t *);
char* i2sd(byte b);
char* dateStamp();
char* timeStamp();

// set constants:
#define NUM_CHANNELS 5      // the number of monitored circuits
#define AC_IN_PIN 2         // the pin connected to a squared 50Hz waveform
#define RESET_PIN 3 		    // restart slave with master
#define OVER_SAMPLE 4       // oversample to remove nonharmonic noise
#define AVR_CLK_ERR 0.993   // to be plotted over time for accuracy

// set variables:
uint8_t SPIbuf[6][32];      // sized to allow waveform transmission to slave
uint8_t bufferNum;					// smooth[0] is voltage, smooth[1->] are current
uint8_t osCount = 0;        // OS oversample count
uint8_t numSamples = 192;	  // number of 104us samples per power cycle
uint8_t SPIoff;             // offset into SPIbuf
uint32_t t0,t1,t2,t3,t4;            // various ms timers
uint32_t loopStart,scanStart,calcStart;           
volatile uint8_t bufferPtr;         // Offset in ADC buffer
volatile uint16_t value;		        // most recent ADC value
volatile uint16_t ADCbuf[200];      // new ADC buffer to allow oversampling

bool bufferFull = false;
uint8_t data[33];
char d2Str[3],fltStr[12];
char dateStr[] = "yymmdd";
char timeStr[] = "hh:mm:ss";
char charBuf[128];
float Voff,Ioff;				          // average ADC offset
float Vsmooth[3][200];            // Storage for smoothed voltage counts
uint16_t Ismooth[10][200];        // Storage for smoothed CURRENT counts
float Wrms[12];		                // Sum of sampled V*I
float Irms[12];			              // Sum of sampled I*I
float Vrms;						            // Sum of sampled V*V
float powerSum;
float IrmsSum;
float VrmsSum;
float volts,amps,Vmax,Vmin,Imax,Imin;
float gridFreq;
