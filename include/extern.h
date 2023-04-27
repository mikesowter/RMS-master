#include <arduino.h>

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

void calcValues();
void sendValues();
void initADC();
void setupSPI();
void getSlaveTime();
void waitForXing();
float getFreq();
void printBuffers();
void send(uint8_t* data);
char* dateStamp();
char* timeStamp();
void flashLEDs();
void setBlue();
void setGreen();
void setRed();

// set constants:
#define NUM_CIRCUITS 8      // the total number of analogs channels (V and Is)
#define AC_IN_PIN 2         // the pin connected to a squared 50Hz waveform
#define SPI_ON_PIN 3    	  // the pin displaying SPI activity
#define CAPTURE_PIN 4		    // displaying capture period
#define RESET_PIN 5			    // displaying any fault
#define RED_PIN 12
#define GRN_PIN 11
#define BLU_PIN 10
#define OVER_SAMPLE 4       // oversample to remove nonharmonic noise
#define AVR_CLK_ERR 0.99936 // updated 20220702 based on avg freq of 49.994
#define vScale 1.423        // updated 20230427 
#define iScale 0.06406      // updated 20200524

// set globals:
extern uint8_t bufferNum;			      // 0-2 are voltage, 3-> are current
extern uint8_t OSindex;             // OS oversample count
extern uint8_t numSamples;			    // number of 104us samples per power cycle
extern uint8_t SPIbuf[][32];        // sized to allow waveform transmission to slave
extern uint8_t SPIoff;              // offset into SPIbuf
extern uint8_t data[];              // 32 bytes from SPI
extern uint32_t t0,t1,t2;           // times reported by slave
extern volatile uint8_t bufferPtr;	// Offset in ADC buffer
extern volatile uint16_t value;			// most recent ADC value
extern volatile uint16_t ADCbuf[];  // new ADC buffer to allow oversampling
extern bool bufferFull, pwrOutage;
extern char d2Str[];
extern char fltStr[];
extern char dateStr[];
extern char timeStr[];
extern char charBuf[];
extern float Vsmooth[3][200];             // Storage for over-sampled voltage counts
extern uint16_t Ismooth[10][200];         // Storage for over-sampled CURRENT counts
extern float Voff,Ioff;				            // average ADC offset
extern float Wrms[];					            // Sum of sampled V*I
extern float Irms[];					            // Sum of sampled I*I
extern float Vrms;						            // Sum of sampled V*V
extern float powerSum;
extern float IrmsSum;
extern float VrmsSum;
extern float volts,amps,Vmax,Vmin,Imax,Imin,Vpk_neg,Vpk_pos;
extern float gridFreq;