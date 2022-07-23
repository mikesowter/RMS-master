#include <arduino.h>

#define RED_PIN 12
#define GRN_PIN 10
#define BLU_PIN 11

void allOff();
void setBlue();
void setGreen();
void setRed();

void flashLEDs() {
  pinMode(BLU_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  setBlue();
  delay(300);
  setGreen();
  delay(300);
  setRed();
  delay(300);
  allOff();
}

void allOff() {
  digitalWrite(BLU_PIN,1);      //turn on Blue
  digitalWrite(GRN_PIN,1);      //turn off Green
  digitalWrite(RED_PIN,1);      //turn off RED_PIN
}

void setBlue() {
  digitalWrite(BLU_PIN,0);      //turn on Blue
  digitalWrite(GRN_PIN,1);      //turn off Green
  digitalWrite(RED_PIN,1);      //turn off RED_PIN
}

void setGreen() {
  digitalWrite(BLU_PIN,1);      //turn off Blue
  digitalWrite(GRN_PIN,0);      //turn on Green
  digitalWrite(RED_PIN,1);      //turn off RED_PIN
}

void setRed() {
  digitalWrite(BLU_PIN,1);      //turn off Blue
  digitalWrite(GRN_PIN,1);      //turn off Green
  digitalWrite(RED_PIN,0);      //turn on RED_PIN
}

void setWhite() {
  digitalWrite(BLU_PIN,0);      //turn on Blue
  digitalWrite(GRN_PIN,0);      //turn on Green
  digitalWrite(RED_PIN,0);      //turn on RED_PIN
}

/* for witty board
void setled(String colour){
  analogWrite(RED_PIN, 0);
  analogWrite(green, 0);
  analogWrite(blue, 0);
  if(colour == "RED_PIN"){
   analogWrite(RED_PIN, 255);
  }

  if(colour == "green"){
   analogWrite(green, 255);
  }

  if(colour == "blue"){
   analogWrite(blue, 255);
  }

  if(colour == "yellow"){
   analogWrite(RED_PIN, 255);
   analogWrite(green, 255);
  }

  if(colour == "cyan"){
   analogWrite(green, 255);
   analogWrite(blue, 255);
  }

  if(colour == "magenta"){
   analogWrite(RED_PIN, 255);
   analogWrite(blue, 255);
  }

  if(colour == "white"){
   analogWrite(RED_PIN, 255);
   analogWrite(green, 255);
   analogWrite(blue, 255);
  }
} */
