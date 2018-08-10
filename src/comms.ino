/*************************************************************
project: RMS
author: M Sowter
description: send values to RMS slave via SPI
*************************************************************
Connect the SPI Master device to the following pins on the esp8266:

    GPIO    NodeMCU   Name  |   Uno
   ===================================
     15       D8       SS   |   D10
     13       D7      MOSI  |   D11
     12       D6      MISO  |   D12
     14       D5      SCK   |   D13

    Note: If the ESP is booting at a moment when the SPI Master has the Select line HIGH (deselected)
    the ESP8266 WILL FAIL to boot!
    See SPISlave_SafeMaster example for possible workaround

*/
#include <SPI.h>

class SPImaster {
  private:
    uint8_t _ss_pin;

  public:
    SPImaster(uint8_t pin): _ss_pin(pin) {}
    void begin() {
      pinMode(_ss_pin, OUTPUT);
      digitalWrite(_ss_pin, HIGH);
    }

    uint32_t readStatus() {
      digitalWrite(_ss_pin, LOW);
      SPI.transfer(0x04);
      uint32_t status = (SPI.transfer(0) | ((uint32_t)(SPI.transfer(0)) << 8) | ((uint32_t)(SPI.transfer(0)) << 16) | ((uint32_t)(SPI.transfer(0)) << 24));
      digitalWrite(_ss_pin, HIGH);
      return status;
    }

    void writeStatus(uint32_t status) {
      digitalWrite(_ss_pin, LOW);
      SPI.transfer(0x01);
      SPI.transfer(status & 0xFF);
      SPI.transfer((status >> 8) & 0xFF);
      SPI.transfer((status >> 16) & 0xFF);
      SPI.transfer((status >> 24) & 0xFF);
      digitalWrite(_ss_pin, HIGH);
    }

    void readData(uint8_t * data) {
      digitalWrite(_ss_pin, LOW);
      SPI.transfer(0x03);
      SPI.transfer(0x00);
      for (uint8_t i = 0; i < 32; i++) {
        data[i] = SPI.transfer(0);
      }
      digitalWrite(_ss_pin, HIGH);
    }

    void writeData(uint8_t * data, size_t len) {
      uint8_t i = 0;
      digitalWrite(_ss_pin, LOW);
      SPI.transfer(0x02);
      SPI.transfer(0x00);
      while (len-- && i < 32) {
        SPI.transfer(data[i++]);
      }
      while (i++ < 32) {
        SPI.transfer(0);
      }
      digitalWrite(_ss_pin, HIGH);
    }

    String readData() {
      char data[33];
      data[32] = 0;
      readData((uint8_t *)data);
      return String(data);
    }

    void writeData(const char * data) {
      writeData((uint8_t *)data, strlen(data));
    }
};

SPImaster SPIm(SS);

void send(const char * message) {
  Serial.print("Master: ");
  Serial.println(message);
  SPIm.writeData(message);
  delay(10);
  Serial.print("Slave: ");
  Serial.println(SPIm.readData());
  Serial.println();
}

void setupSPI() {
  SPISettings set(2000000, LSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPIm.begin();
  delay(1000);
  send("Hello Slave!");
}

void sendValues() {
  buff[0] = packetNum;
  buff[1] = (uint16_t)(Vrms*10.0);
  for (int i = 0; i<7; i++) {
    buff[2 * i + 2] = (uint16_t)(Irms[1]*1000.0);
    buff[2 * i + 3] = (uint16_t)(power[1]*10.0);
  }
//  printf("buff[0-3]: %x %d %d %d\n\r\r",buff[0],buff[1],buff[2],buff[3]);
  if (numChannels>8) {
    for (int i = 8; i<numChannels; i++) {
      buff[2 * i ] = (uint16_t)(Irms[i+1]*1000.0);
      buff[2 * i + 1] = (uint16_t)(power[i+1]*10.0);
    }
  }
  if (++packetNum > 0x2000) packetNum = 0x1000;
}
