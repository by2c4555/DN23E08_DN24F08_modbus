#include <SD.h>


//Author: eletechsup
//More information welcome to : http://www.eletechsup.com 
//Arduino 0022
//for Arduino NANO
//4 bit Common Cathode Digital Tube Module test code,use Timer2
//This code is used to verify: nixie tube/relay/input port/button

/*---Segment Display Screen----
--A--
F---B
--G--
E---C
--D-- 
 __  __   __  __
|__||__| |__||__|
|__||__| |__||__|
----------------------*/
#include <FlexiTimer2.h>
#define uchar unsigned char 
#define uint  unsigned int

int display_dat; 
int key_value;

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


const int RS485_RD =13;

int relay1_delay_time = 1;//1-9999 seconds,Modify the number change the delay time
int relay2_delay_time = 2;//1-9999 seconds,Modify the number change the delay time
int relay3_delay_time = 3;//1-9999 seconds,Modify the number change the delay time
int relay4_delay_time = 4;//1-9999 seconds,Modify the number change the delay time
int relay5_delay_time = 5;//1-9999 seconds,Modify the number change the delay time
int relay6_delay_time = 6;//1-9999 seconds,Modify the number change the delay time
int relay7_delay_time = 7;//1-9999 seconds,Modify the number change the delay time
int relay8_delay_time = 8;//1-9999 seconds,Modify the number change the delay time9
int relay1_time_left;
int relay2_time_left;
int relay3_time_left;
int relay4_time_left;
int relay5_time_left;
int relay6_time_left;
int relay7_time_left;
int relay8_time_left;

int relay1_display;
int relay2_display;
int relay3_display;
int relay4_display;
int relay5_display;
int relay6_display;
int relay7_display;
int relay8_display;



/*       NO.:0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22 23 24 25 26 27 28      
/*Character :0,1,2,3,4,5,6,7,8,9,A, b, C, c, d, E, F, H, h, L, n, N, o, P, r, t, U, -,  ,*/
uchar  TUBE_SEG[] = 
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x58,0x5e,0x79,0x71,0x76,0x74,0x38,0x54,0x37,0x5c,0x73,0x50,0x78,0x3e,0x40,0x00};//Common anode Digital Tube Character Gallery

uchar TUBE_NUM[8]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//Tuble bit number

uchar dat_buf[8];

uchar InputData;

uchar ReadByte_165(){
  uchar byte_temp = 0;
  uchar byte_;
  
     // Write pulse to load pin
  digitalWrite(LOAD_165, LOW);
  delayMicroseconds(5);
  digitalWrite(LOAD_165, HIGH);
  
   // Get data from 74HC165
    for (int i = 7;i>=0;i--){
    byte_ = digitalRead(DATA_165);
    byte_temp |= (byte_ << i);
    digitalWrite(CLK_165,HIGH);
    delayMicroseconds(5);
    digitalWrite(CLK_165,LOW);
    }
    
  return ~byte_temp;
 }

//***********************************************
// dat : Data to be displayed
// com_num :  Digital Tube Common 
//relay_port : Relay port
//***********************************************
uchar dat;
uchar com_num;
uchar relay_port;
void TubeDisplayOneBit()
{
  uchar tube_dat,bit_num;
  uchar display_l,display_h,relay_dat;
   
   tube_dat = TUBE_SEG[dat]; //Common Cathode Digital Tube,bit negated
   bit_num = TUBE_NUM[com_num];
   
   relay_dat = relay_port;
   
     //ground latchPin and hold low for as long as you are transmitting
     digitalWrite(latchPin, LOW);  
     
     shiftOut(dataPin, clockPin, MSBFIRST, relay_dat); 
     shiftOut(dataPin, clockPin, MSBFIRST, bit_num); 
     shiftOut(dataPin, clockPin, MSBFIRST, tube_dat);

     //return the latch pin high to signal chip that it 
     //no longer needs to listen for information
     digitalWrite(latchPin, HIGH);
}

uint OneSecondCnt=500;
void TubeDisplay4Bit(void)
{
  if(com_num<7) com_num ++;else com_num=0;
  dat = dat_buf[com_num];
  TubeDisplayOneBit();

  OneSecondCnt--;
  if(OneSecondCnt==0)
  {
    OneSecondCnt = 500;
    if(relay1_time_left>0){relay1_time_left--; relay1_display = relay1_time_left;} else {bitClear(relay_port,0);relay1_display = relay1_delay_time;}
    if(relay2_time_left>0){relay2_time_left--; relay2_display = relay2_time_left;} else {bitClear(relay_port,1);relay2_display = relay2_delay_time;}
    if(relay3_time_left>0){relay3_time_left--; relay3_display = relay3_time_left;} else {bitClear(relay_port,2);relay3_display = relay3_delay_time;}
    if(relay4_time_left>0){relay4_time_left--; relay4_display = relay4_time_left;} else {bitClear(relay_port,3);relay4_display = relay4_delay_time;}
    if(relay5_time_left>0){relay5_time_left--; relay5_display = relay5_time_left;} else {bitClear(relay_port,4);relay5_display = relay5_delay_time;}
    if(relay6_time_left>0){relay6_time_left--; relay6_display = relay6_time_left;} else {bitClear(relay_port,5);relay6_display = relay6_delay_time;}
    if(relay7_time_left>0){relay7_time_left--; relay7_display = relay7_time_left;} else {bitClear(relay_port,6);relay7_display = relay7_delay_time;}
    if(relay8_time_left>0){relay8_time_left--; relay8_display = relay8_time_left;} else {bitClear(relay_port,7);relay8_display = relay8_delay_time;}
   
  }

 
  if(key_value==0) display_dat = relay1_display; 
  else if(key_value==1) display_dat = relay2_display;
  else if(key_value==2) display_dat = relay3_display;  
  else if(key_value==3) display_dat = relay4_display;

   /*
  if(key_value==0) display_dat = relay5_display; 
  else if(key_value==1) display_dat = relay6_display;
  else if(key_value==2) display_dat = relay7_display;  
  else if(key_value==3) display_dat = relay8_display;
    */
    
   dat_buf[0] = display_dat/1000;
   display_dat = display_dat%1000;
   dat_buf[2] = display_dat/100;
   display_dat = display_dat%100;
   dat_buf[4] = display_dat/10;
   dat_buf[6] = display_dat%10;
  
}

void setup() {
   //set pins to output because they are addressed in the main loop
   
   pinMode(latchPin, OUTPUT);
   pinMode(clockPin, OUTPUT);
   pinMode(dataPin, OUTPUT);
   pinMode(OE_595, OUTPUT);
   
   pinMode(K1, INPUT);
   pinMode(K2, INPUT);
   pinMode(K3, INPUT);
   pinMode(K4, INPUT);
   
   pinMode(LOAD_165, OUTPUT);
   pinMode(DATA_165, INPUT);
   pinMode(CLK_165, OUTPUT);
   

  FlexiTimer2::set(1, 1.0/1000, TubeDisplay4Bit); // call every 2ms "ticks"
  FlexiTimer2::start();
  
  
  digitalWrite(K1, HIGH);
  digitalWrite(K2, HIGH);
  digitalWrite(K3, HIGH);
  digitalWrite(K4, HIGH);
  
  key_value = 0;
  
  digitalWrite(OE_595, LOW);// Enable 74HC595  
}

void loop() {
 
  while(1)
  {
    
    InputData = ReadByte_165();

    if(InputData&0x01) {relay1_time_left = relay1_delay_time;bitSet(relay_port,0);}

    if(InputData&0x02) {relay2_time_left = relay2_delay_time;bitSet(relay_port,1);}

    if(InputData&0x04) {relay3_time_left = relay3_delay_time;bitSet(relay_port,2);}

    if(InputData&0x08) {relay4_time_left = relay4_delay_time;bitSet(relay_port,3);}
    
    if(InputData&0x10) {relay5_time_left = relay5_delay_time;bitSet(relay_port,4);}
    
    if(InputData&0x20) {relay6_time_left = relay6_delay_time;bitSet(relay_port,5);}
    
    if(InputData&0x40) {relay7_time_left = relay7_delay_time;bitSet(relay_port,6);}
    
    if(InputData&0x80) {relay8_time_left = relay8_delay_time;bitSet(relay_port,7);}
    
    if     (digitalRead(K1)==LOW) {key_value = 0;} 
    else if(digitalRead(K2)==LOW) {key_value = 1;} 
    else if(digitalRead(K3)==LOW) {key_value = 2;} 
    else if(digitalRead(K4)==LOW) {key_value = 3;}    
  }
} 
