/*************************************************************
project: RMS
author: M Sowter
description: send values to master station via radio
*************************************************************/

extern const uint8_t numChannels;                   // number of analogs (V+I)
extern const uint8_t nums;			                    // number of samples per power cycle
extern uint8_t bufferNum;				                    // selects buffer and analog mux (0-7)
extern volatile uint16_t buffer[numChannels][200];	// filled under interrupt from ADCH register
extern volatile uint8_t bufferPtr;				          // Offset in current buffer

extern float power[numChannels];					      // Sum of sampled V*I
extern float Irms[numChannels];					       	// Sum of sampled I*I
extern float Vrms;						        // Sum of sampled V*V

void sendValues() {
  buff[0] = packetNum;
  buff[1] = (uint16_t)(Vrms*10.0);
  for (int i = 0; i<7; i++) {
    buff[2 * i + 2] = (uint16_t)(Irms[i+1]*1000.0);
    buff[2 * i + 3] = (uint16_t)(power[i+1]*10.0);
  }
  printf("buff[0-3]: %x %d %d %d\n\r\r",buff[0],buff[1],buff[2],buff[3]);
  for (; radio.write(buff, 32) == false;){};  // first 32 byte buffer contains V & circuits 1-7
  if (numChannels>8) {
    for (int i = 8; i<numChannels; i++) {
      buff[2 * i ] = (uint16_t)(Irms[i+1]*1000.0);
      buff[2 * i + 1] = (uint16_t)(power[i+1]*10.0);
    }
    for (; radio.write(buff, 32) == false;){
      printf("-%d",millis());
    };  // second 32 byte buffer contains circuits 8-15
  }
  if (++packetNum > 0x2000) packetNum = 0x1000;
}

void radioSetup()
{
	radio.begin();
  radio.setPayloadSize(32);
	radio.setAutoAck(false);
	//radio.enableAckPayload();
	radio.setDataRate(RF24_250KBPS);
	radio.setRetries(15, 15);
	radio.setChannel(90);
	radio.openWritingPipe(pipes[1]);
	radio.printDetails();
}
