#include "DN23E08_IO.h"
#include <ModbusRTUSlave.h>
#define MODBUS_SERIAL Serial
#define MODBUS_BAUD 9600
#define MODBUS_CONFIG SERIAL_8N1
#define MODBUS_UNIT_ID 1
#define dePin 13
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
// Onboard KEY1-KEY2-KEY3-KEY4
const int K1 = 5;
const int K2 = 6;
const int K3 =7;
const int K4 = 8;

DN23E08_IO gpio(latchPin,clockPin,dataPin,OE_595,LOAD_165,DATA_165,CLK_165);
int count = 0;
void DN23E08Init(){
  gpio.begin();
  gpio.RelayClear(2);
  pinMode(K1, INPUT);
  pinMode(K2, INPUT);
  pinMode(K3, INPUT);
  pinMode(K4, INPUT);
}
ModbusRTUSlave modbus(MODBUS_SERIAL, dePin);
const uint8_t numCoils = 8;
const uint8_t numDiscreteInputs = 8;
const uint8_t numHoldingRegisters = 2;
const uint8_t numInputRegisters = 8;
bool coils[numCoils];
bool discreteInputs[numDiscreteInputs];
uint16_t holdingRegisters[numHoldingRegisters];
uint16_t inputRegisters[numInputRegisters];

void ModbusInit(){
  modbus.configureCoils(coils, numCoils);
  modbus.configureDiscreteInputs(discreteInputs, numDiscreteInputs);
  modbus.configureHoldingRegisters(holdingRegisters, numHoldingRegisters);
  modbus.configureInputRegisters(inputRegisters, numInputRegisters);
  MODBUS_SERIAL.begin(MODBUS_BAUD, MODBUS_CONFIG);
  modbus.begin(MODBUS_UNIT_ID, MODBUS_BAUD, MODBUS_CONFIG);
}
  
void ModbusUpdate(){
  gpio.WirteCoils(coils,numCoils);
  gpio.ReadDiscreteInputs(discreteInputs,numDiscreteInputs);
  gpio.SetDisplay((int)holdingRegisters[0]);
}

void setup() {
  ModbusInit();
  DN23E08Init();
  gpio.OffDisplay();
}

void loop(){
  delay(200);
  ModbusUpdate();
  modbus.poll(); 
}



