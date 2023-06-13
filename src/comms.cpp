/*************************************************************
project: RMS
author: M Sowter
description: send values to RMS slave via SPI
*************************************************************
Connect the SPI Master device via the following pins:

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
uint16_t checkSum;

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
      flashPIN(Rx_PIN);
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
      flashPIN(Tx_PIN);
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
  checkSum += ii;
}

void loadValues() {
  // always send battery voltage
  

  if ( pwrOutage ) {              // signal power outage to slave 
    SPIbuf[0][0] = 0xFF;
    SPIbuf[0][1] = 0xFF;
    Serial.print((char*) dateStamp());
    Serial.print(" ");
    Serial.print((char*) timeStamp());  
    Serial.println("  Power Outage");
    SPIoff = 28;
    load2Bytes((float)analogRead(A15)*16.63);     // battery voltage 3V-4.2V
    Serial.println((float)analogRead(A15)/90.4);
  }
  else {
    SPIoff = 0;
    checkSum = 0;
    load2Bytes(getFreq()*1000.0);
    load2Bytes(Vrms*100.0);
    load2Bytes(Vpk_neg*50.0);
    load2Bytes(Vpk_pos*50.0);
    Serial.print((char*) timeStamp());  
    for (uint8_t p=1 ; p<=NUM_CIRCUITS ; p++) {   // bytes 8-27 allow for 10 circuits
      load2Bytes(Wrms[p]);
      Serial.print(",");
      Serial.print(Wrms[p]);
    }
    SPIoff = 28;
    load2Bytes((float)analogRead(A15)*17.15);     // battery voltage 3V-4.2V
    Serial.print(",");
    Serial.println((float)analogRead(A15)/124.6);
    load2Bytes((float)checkSum);
  }
   
}

void getSlaveTime() {
  uint32_t start = millis();
  while (year() < 2022 || year() > 2050) {
    setTime(SPIm.readStatus()); 
    Serial.print(year());
    Serial.print(" ");
    delay(1000);
    if (millis()-start < 30000) continue;
    setRed();
  }
  Serial.print("\ntime code received in ");
  Serial.print(millis()-start);
  Serial.print(" ms\n");
  Serial.print((char*) dateStamp());
  Serial.print(" ");
  Serial.println((char*) timeStamp());  
  delay(10);
}

