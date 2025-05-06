#include "DN23E08_IO.h"
//Pin connected to latch of Digital Tube Module
int latchPin = 11;
//Pin connected to clock of Digital Tube Module
int clockPin = 12;
//Pin connected to data of Digital Tube Module
int dataPin = 9;
//Pin connected to 595_OE of Digital Tube Module
int OE_595 = 10;
// Define Connections to 74HC165
 // LOAD pin 
 int LOAD_165 = 4;
 // DATA165 Pin
 int DATA_165 = 2;
 //CLK pin
 int CLK_165 = 3;


const int K1 = 5;
const int K2 = 6;
const int K3 =7;
const int K4 = 8;

DN23E08_IO gpio(latchPin,clockPin,dataPin,OE_595,LOAD_165,DATA_165,CLK_165);

int count = 0;

void setup() {

  Serial.begin(9600);
  gpio.begin();
  gpio.RelayClear(2);

  pinMode(K1, INPUT);
  pinMode(K2, INPUT);
  pinMode(K3, INPUT);
  pinMode(K4, INPUT);
 
}

void loop() {
 


  if     (gpio.Read165INP(0)) {gpio.RelaySet(0);} 
  else if(gpio.Read165INP(1)) {gpio.RelaySet(1);}
  else if(gpio.Read165INP(2)) {gpio.RelaySet(2);}
  else if(gpio.Read165INP(3)) {gpio.RelaySet(3);}
  else{gpio.RelayClearAll();}
  
  delay(1000);
  gpio.RelayClear(2);
  gpio.SetDisplay(count);
  count++;


}



