#include <Arduino.h>

/*************************************************************
project: RMS
author: M Sowter
description: send values to master station via radio
*************************************************************

#include <TimeLib.h>

int summaryBuff[32];
byte traceBuff[32];

byte requestTime() {
  int i=0;
  Serial.print("requesting time: ");
  while (i==0) {
    Wire.beginTransmission(0x66);    // transmit to device #8 (ESP-12)
    Wire.write("T");
    Wire.endTransmission();
/*    Wire.requestFrom(8, 6);       // request 6 bytes from slave device #8

    while (Wire.available()) {    // slave may send less than requested
      IObuff[i++] = Wire.read();  // receive a byte as character
    }
  delay(1);
  Serial.print('.');
  }
    Serial.print(i);
    Serial.println(" bytes received");

  return i;
}

void sendValues() {
  summaryBuff[0] = packetNum;
  summaryBuff[1] = Vrms;
  for (int i = 0; i<7; i++)
  {
    summaryBuff[2 * i + 2] = Irms[i];
    summaryBuff[2 * i + 3] = power[i];
  }

    if (packetNum++ > 32000) packetNum = 10000;
}
*/
