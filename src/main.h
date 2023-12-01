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
void transport();
char* i2sd(byte b);
char* dateStamp();
char* timeStamp();
void flashLEDs();
void flashPIN(uint8_t pin);
void setBlue();
void setGreen();
void setRed();
void allOff();

// set constants:
#define NUM_CCTS 8      
#define Tx_PIN 0
#define Rx_PIN 1
#define AC_IN_PIN 2         
#define RESET_PIN 3 		 
#define RED_PIN 12
#define BLU_PIN 11
#define GRN_PIN 10
#define OVER_SAMPLE 1       // oversample to remove nonharmonic noise - set to no oversample

// set variables:
uint8_t MOSIbuf[32], MISObuf[32];      // separate buffers
uint8_t bufferNum;					// smooth[0] is voltage, smooth[1->] are current
uint8_t osCount = 0;        // OS oversample count
uint8_t numSamples = 192;	  // number of 104us samples per power cycle
uint8_t SPIoff;             // offset into MOSIbuf
uint32_t t0,t1,t2,t3,t4;            // various ms timers
uint32_t loopStart,scanStart,calcStart;           
volatile uint8_t bufferPtr;         // Offset in ADC buffer
volatile uint16_t value;		        // most recent ADC value
volatile uint16_t ADCbuf[200];      // new ADC buffer to allow oversampling

bool bufferFull = false;
bool pwrOutage = false;
uint8_t data[33];
char d2Str[3],fltStr[12];
char dateStr[] = "yymmdd";
char timeStr[] = "hh:mm:ss";
char charBuf[128];
float Voff,Ioff;				          // average ADC offset
float V[3][200];            // Storage for smoothed voltage counts
uint16_t Ismooth[10][200];        // Storage for smoothed CURRENT counts
float Wrms[10];		                // Sum of sampled V*I
float Irms[10];			              // Sum of sampled I*I
float Vrms;						            // Sum of sampled V*V
float powerSum;
float IrmsSum;
float VrmsSum;
float volts,amps,Vmax,Vmin,Imax,Imin,Vnp_min,Vpp_max;
float gridFreq;
