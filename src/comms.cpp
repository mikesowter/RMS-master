/*************************************************************
project: RMS
author: M Sowter
description: send values to RMS slave via SPI
*************************************************************
Connect the SPI Master device via the following pins:

    GPIO    esp8266   Name  |   Uno     Mega2560
   =============================================
     15       D8       SS   |   D10       53   BLUE
     13       D7      MOSI  |   D11       51   YELLOW
     12       D6      MISO  |   D12       50   WHITE
     14       D5      SCK   |   D13       52   GREEN

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
    //  flashPIN(Rx_PIN);
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
      if (pwrOutage) flashPIN(RED_PIN);
      else flashPIN(GRN_PIN);
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
  Serial.print(ii,HEX);
  Serial.print(" ");
}

void loadValues() {
  Serial.print("\n");
  Serial.print(timeStamp());  
  SPIoff = 0;
  checkSum = 0;
  if ( pwrOutage ) {              // signal power outage to slave 
    load2Bytes(0xFFFF);                 
  }
  else {
    load2Bytes(getFreq()*500.0);
    load2Bytes(Vrms*100.0);
    for (uint8_t p=1 ; p<=NUM_CCTS ; p++) {     // bytes 4-25 allow for 11 circuits
      load2Bytes(Wrms[p]);
    }
    SPIoff = 26;
    load2Bytes(Vpp_max*50.0);
    load2Bytes(Vnp_min*50.0);
  }
  SPIoff = 30;
  load2Bytes((float)checkSum);                  // for power outage as well
}

void getSlaveTime() {
  uint32_t start = millis();
  while (year() < 2022 || year() > 2050) {
    setTime(SPIm.readStatus()); 
  //  Serial.print(year());
  //  Serial.print(" ");
    delay(1000);
    if (millis()-start > 10000) {
      Serial.print("\ntime code not received");
      return;
    }
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

