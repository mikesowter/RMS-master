#include <arduino.h>

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

void calcValues();
void initADC();
void setupSPI();
void getSlaveTime();
void waitForXing();
float getFreq();
void printBuffers();
void transport();
char* dateStamp();
char* timeStamp();
void flashLEDs();
void flashPIN(uint8_t pin);
void setBlue();
void setGreen();
void setRed();

// set constants:
#define NUM_CCTS 8          // the total number of analogs channels (V and Is)
#define Tx_PIN 0
#define Rx_PIN 1
#define AC_IN_PIN 2         // the pin connected to a squared 50Hz waveform
#define SPI_ON_PIN 3    	// the pin displaying SPI activity
#define CAPTURE_PIN 4		// displaying capture period
#define RESET_PIN 5			// displaying any fault
#define RED_PIN 12
#define BLU_PIN 11
#define GRN_PIN 10
#define OVER_SAMPLE 4       // oversample to remove nonharmonic noise
#define vScale 1.459        // updated 20230806 
#define iScale 0.06406      // updated 20200524

// set globals:
extern uint8_t bufferNum;			      // 0-2 are voltage, 3-> are current
extern uint8_t OSindex;             // OS oversample count
extern uint8_t numSamples;			// number of 104us samples per power cycle
extern uint8_t MOSIbuf[], MISObuf[];            
extern uint8_t SPIoff;              // offset into MOSIbuf
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
extern float V[3][200];             // Storage for over-sampled voltage counts
extern uint16_t Ismooth[10][200];         // Storage for over-sampled CURRENT counts
extern float Voff,Ioff;				            // average ADC offset
extern float Wrms[];					            // Sum of sampled V*I
extern float Irms[];					            // Sum of sampled I*I
extern float Vrms;						            // Sum of sampled V*V
extern float powerSum;
extern float IrmsSum;
extern float VrmsSum;
extern float volts,amps,Vmax,Vmin,Imax,Imin,Vnp_min,Vpp_max;
extern float gridFreq;