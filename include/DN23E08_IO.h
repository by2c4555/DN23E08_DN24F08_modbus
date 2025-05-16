#include <Arduino.h>
#include <SD.h>
#include <FlexiTimer2.h>

class DN23E08_IO {
private:
  static DN23E08_IO* _instance;  // Static pointer to current instance

  const int _latchPin;
  const int _clockPin;
  const int _dataPin;
  const int _OE_595;
  const int _load165Pin;
  const int _data165Pin;
  const int _clk165Pin;

  unsigned char FlexiTimer2_dealy_counter_1;

  /*       NO.:0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22 23 24 25 26 27 28      
  /*Character :0,1,2,3,4,5,6,7,8,9,A, b, C, c, d, E, F, H, h, L, n, N, o, P, r, t, U, -,  ,*/
  unsigned char  TUBE_SEG[29] = {
    0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
    0x77,0x7c,0x39,0x58,0x5e,0x79,0x71,0x76,0x74,0x38,
    0x54,0x37,0x5c,0x73,0x50,0x78,0x3e,0x40,0x00
  };

  unsigned TUBE_CULUM[8] = {
    0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f
  };
  unsigned char relay_port;
  
  volatile unsigned char dat[4] = {28,28,28,28};
  volatile unsigned char com_num;
  volatile uint16_t adcResults[8]; // Stores results for ADC0-ADC7
  volatile uint8_t adcCurrentChannel = 0;

  unsigned char Read165INP_byte_cur;
  unsigned char Read165INP_byte_buff;
  unsigned char  Read165INP_byte_pre;


  // This is now a static wrapper
  static void CallbackWrapper() {
    if (_instance) {
      _instance->FlexiTimer2Callback();  // Call actual instance method
    }
  }


  void Read165INP_fnc(){
    // Write pulse to load pin
   Read165INP_byte_cur = 0;
  digitalWrite(_load165Pin, LOW);
  delayMicroseconds(5);
  digitalWrite(_load165Pin, HIGH);
  
   // Get data from 74HC165
    for (int i = 7;i>=0;i--){
    Read165INP_byte_buff = digitalRead(_data165Pin);
    Read165INP_byte_cur |= (Read165INP_byte_buff << i);
    digitalWrite(_clk165Pin,HIGH);
    delayMicroseconds(5);
    digitalWrite(_clk165Pin,LOW);
    }
    Read165INP_byte_pre = ~Read165INP_byte_cur;
 }

  void TubeDisplayOneBit() {
    unsigned char tube_dat, bit_num, relay_dat;
    if (com_num > 3) com_num = 0;

    tube_dat = TUBE_SEG[dat[com_num]];
    bit_num = TUBE_CULUM[com_num];
    com_num++;
    relay_dat = relay_port;

    digitalWrite(_latchPin, LOW);

    shiftOut(_dataPin, _clockPin, MSBFIRST, relay_dat);
    shiftOut(_dataPin, _clockPin, MSBFIRST, bit_num);
    shiftOut(_dataPin, _clockPin, MSBFIRST, tube_dat);

    digitalWrite(_latchPin, HIGH);
  }

  void ReadAnalogInput(){
        // Check if a conversion is complete (ADIF flag set)
    if (ADCSRA & (1 << ADIF)) {
      adcResults[adcCurrentChannel] = ADC; // Read result (clears ADIF)
      
      // Switch to next channel (0-7)
      adcCurrentChannel = (adcCurrentChannel + 1) % 8;
      ADMUX = (ADMUX & 0xF0) | adcCurrentChannel; // Update MUX
      
      ADCSRA |= (1 << ADSC); // Restart conversion (auto-triggered in free-running)
    }
  }

  void FlexiTimer2Callback(){
    // call every 3 ms
    TubeDisplayOneBit();  
    ReadAnalogInput();
    if(FlexiTimer2_dealy_counter_1 > 200){
      FlexiTimer2_dealy_counter_1 = 0 ;
      Read165INP_fnc();  // call every 600 ms 
    }
    FlexiTimer2_dealy_counter_1++;
  }



public:
DN23E08_IO(int latch595Pin, int clock595Pin, int data595Pin, int OE595Pin,int load165Pin , int data165Pin, int clk165Pin)
    : _latchPin(latch595Pin),
      _clockPin(clock595Pin),
      _dataPin(data595Pin),
      _OE_595(OE595Pin),
      _load165Pin(load165Pin),
      _data165Pin(data165Pin),
      _clk165Pin(clk165Pin)
      {
    
    _instance = this;

    pinMode(_latchPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);
    pinMode(_OE_595, OUTPUT);
    pinMode(_load165Pin, OUTPUT);
    pinMode(_data165Pin, INPUT);
    pinMode(_clk165Pin, OUTPUT);
    relay_port = 0;
    ADC_init(); // Configure ADC Free-running mode
    FlexiTimer2::set(3,CallbackWrapper); // call every 3 ms
  }

  void ADC_init(){
      // Configure ADC
  ADMUX = (1 << REFS0);          // AVcc reference, ADC0 initially
  ADCSRA = (1 << ADEN) |         // Enable ADC
           (1 << ADSC) |         // Start first conversion
           (1 << ADATE) |        // Auto Trigger Enable (free-running)
           (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128 (125 kHz)
  ADCSRB = 0;                    // Free-running mode
  DIDR0 = 0xFF;                  // Disable digital inputs on ADC0-ADC7 (optional)
  }



  void begin(){
    FlexiTimer2::start();
  }

  void RelaySet(const unsigned char portnum){
    if(portnum>7 || portnum < 0 )return;
    // cli();            // Disable interrupts to enter critical section
    bitSet(relay_port,portnum);
    // sei();            // Re-enable interrupts
  }

  void RelayClear(const unsigned char portnum){
    if(portnum>7 || portnum < 0 )return;
    cli();            // Disable interrupts to enter critical section
    bitClear(relay_port,portnum);
    sei();            // Re-enable interrupts
  }

  void RelayClearAll(){
    cli();            // Disable interrupts to enter critical section
    relay_port=0;
    sei();            // Re-enable interrupts
  }

  unsigned char RelayRead(const unsigned char pin){
    return (relay_port & (1 << pin));
  }

  void SetDisplay(unsigned int value){
  if(value < 0 || value > 9999) return;
  cli();            // Disable interrupts to enter critical section
  dat[0] = (value / 1000) % 10;
  dat[1] = (value / 100) % 10;
  dat[2] = (value / 10) % 10;
  dat[3] = value % 10;
  sei();            // Re-enable interrupts
  }
  void OffDisplay(){
    cli();            // Disable interrupts to enter critical section
    dat[0] = 28;
    dat[1] = 28;
    dat[2] = 28;
    dat[3] = 28;
    sei();            // Re-enable interrupts
  }
  unsigned int ReadDisplay(){
    return  dat[0] * 1000 + dat[1] * 100 + dat[2] * 10 + dat[3];
  }
  
  bool Read165INP(unsigned char pin){
    if(pin > 7 || pin <0 ) return false;
    return (Read165INP_byte_pre & (1 << pin));
  } 

  void WirteCoils(bool* coils, size_t length){
    cli();            // Disable interrupts to enter critical section
     relay_port=0;
    for (size_t i = 0; i < length; i++) {
      relay_port |= (coils[i] ? 1 : 0)<<i;  // Example: increment each element
  }
    sei();           // Re-enable interrupts
  }

  void ReadDiscreteInputs(bool* discreteInputs, size_t length){
    cli();            // Disable interrupts to enter critical section
    for (size_t i = 0; i < length; i++) {
      discreteInputs[i] = false;
      discreteInputs[i] |= Read165INP(i);  // Example: increment each element
  }
    sei();  
  }
  
  void ReadInputRegisters(uint16_t* InputRegisters, size_t length){
    cli();            // Disable interrupts to enter critical section
    for (size_t i = 0; i < length; i++) {
      InputRegisters[i] = false;
      InputRegisters[i] = adcResults[i];  // Example: increment each element
  }
    sei(); 
  }
  
};

// Definition of the static instance pointer
DN23E08_IO* DN23E08_IO::_instance = nullptr;