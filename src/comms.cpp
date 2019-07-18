/*************************************************************
project: RMS
author: M Sowter
description: send values to RMS slave via SPI
*************************************************************
Connect the SPI Master device to the following pins on the esp8266:

    GPIO    NodeMCU   Name  |   Uno     Mega2560
   =============================================
     15       D8       SS   |   D10       D19
     13       D7      MOSI  |   D11       D21
     12       D6      MISO  |   D12       D22
     14       D5      SCK   |   D13       D20

    Note: If the ESP is booting at a moment when the SPI Master has the Select line HIGH (deselected)
    the ESP8266 WILL FAIL to boot!
    See SPISlave_SafeMaster example for possible workaround

*/
#include <SPI.h>
#include "extern.h"
#include <Time.h>
uint8_t seq = 0;

class ESPSafeMaster {
  private:
    uint8_t _ss_pin;
    void _pulseSS() {
      digitalWrite(_ss_pin, HIGH);
      delayMicroseconds(5);
      digitalWrite(_ss_pin, LOW);
    }
  public:
    ESPSafeMaster(uint8_t pin): _ss_pin(pin) {}
    void begin() {
      pinMode(_ss_pin, OUTPUT);
      _pulseSS();
    }

    uint32_t readStatus() {
      _pulseSS();
      SPI.transfer(0x04);
      uint32_t status = (SPI.transfer(0) | ((uint32_t)(SPI.transfer(0)) << 8) | ((uint32_t)(SPI.transfer(0)) << 16) | ((uint32_t)(SPI.transfer(0)) << 24));
      _pulseSS();
      return status;
    }

    void writeStatus(uint32_t status) {
      _pulseSS();
      SPI.transfer(0x01);
      SPI.transfer(status & 0xFF);
      SPI.transfer((status >> 8) & 0xFF);
      SPI.transfer((status >> 16) & 0xFF);
      SPI.transfer((status >> 24) & 0xFF);
      _pulseSS();
    }

    void readData(uint8_t * data) {
      _pulseSS();
      SPI.transfer(0x03);
      SPI.transfer(0x00);
      for (uint8_t i = 0; i < 32; i++) {
        data[i] = SPI.transfer(0);
      }
      _pulseSS();
    }

    void writeData(uint8_t * data, size_t len) {
      uint8_t i = 0;
      _pulseSS();
      SPI.transfer(0x02);
      SPI.transfer(0x00);
      while (len-- && i < 32) {
        SPI.transfer(data[i++]);
      }
      while (i++ < 32) {
        SPI.transfer(0);
      }
      _pulseSS();
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

ESPSafeMaster SPIm(SS);

void send(uint8_t* data) {
  SPIm.writeData(data,32);
  delay(10);
  SPIm.readData(data);
  delay(10);
}

void setupSPI() {
  SPI.begin();
  SPIm.begin();
}

void load2Bytes(float f) {
  uint16_t ii = (uint16_t) abs(f);
  SPIbuf[0][SPIoff++] = highByte(ii);
  SPIbuf[0][SPIoff++] = lowByte(ii);
}

void loadValues() {
  SPIoff = 0;
  load2Bytes(getFreq()*1000.0);
  load2Bytes(Vrms*100.0);
  load2Bytes(Vmin*50.0);
  load2Bytes(Vmax*50.0);
  for (uint8_t p=1 ; p<=NUM_CHANNELS ; p++) {   // bytes 8-27
    load2Bytes(Wrms[p]);
    Serial.print("W[");
    Serial.print(p);
    Serial.print("]=");
    Serial.print(Wrms[p]);
    Serial.print(" ");
  }
}

void getSlaveTime() {
  uint32_t start = millis();
  while (year() < 2019 || year() > 2030) {
    setTime(SPIm.readStatus()); 
    Serial.print(year());
    Serial.print(" ");
    delay(200);
    if (millis()-start < 30000) continue;
    setRed();
    digitalWrite(RESET_PIN, 0);		// reset slave
    delay(100);
    digitalWrite(RESET_PIN, 1);
    setTime(0,0,0,1,1,2019);
  }
  Serial.print((char*) dateStamp());
  Serial.print(" ");
  Serial.println((char*) timeStamp());  
  delay(10);
}

