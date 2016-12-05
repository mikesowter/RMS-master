/*************************************************************
project: RMS
author: M Sowter
description: send values to master station via radio
*************************************************************/

uint16_t buff[32];                                  // 32 byte radio data buffer
uint16_t packetNum;				                          // packet sequence identifier

extern const uint8_t numChannels;                   // number of analogs (V+I)
extern const uint8_t nums;			                    // number of samples per power cycle
extern uint8_t bufferNum;				                    // selects buffer and analog mux (0-7)
extern volatile uint16_t buffer[numChannels][200];	// filled under interrupt from ADCH register
extern volatile uint8_t bufferPtr;				          // Offset in current buffer

extern float power[7];					      // Sum of sampled V*I
extern float Irms[7];					       	// Sum of sampled I*I
extern float Vrms;						        // Sum of sampled V*V

void sendValues() {
  buff[0] = packetNum;
  buff[1] = (uint16_t)(Vrms*100.0);
  for (int i = 0; i<7; i++) {
    buff[2 * i + 2] = (uint16_t)(Irms[i]*100.0);
    buff[2 * i + 3] = (uint16_t)power[i];
  }
  for (; radio.write(buff, 32) == false;){};  // first 32 byte buffer contains V & circuits 1-7
  if (numChannels>8) {
    for (int i = 8; i<numChannels; i++) {
      buff[2 * i ] = (uint16_t)(Irms[i]*100.0);
      buff[2 * i + 1] = (uint16_t)power[i];
    }
    for (; radio.write(buff, 32) == false;){};  // second 32 byte buffer contains circuits 8-15
    if (packetNum++ > 50000) packetNum = 10000;
  }
}

void radioSetup()
{
	radio.begin();
	radio.setAutoAck(true);
	radio.enableAckPayload();
	radio.setDataRate(RF24_250KBPS);
	radio.setRetries(15, 15);
	radio.setChannel(90);
	radio.openWritingPipe(pipes[1]);
	radio.printDetails();
}
