#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include "apps.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_PIN     5
#define LED_COUNT   24
#define key         2
#define s2          3
#define s1          4

// Objects declaration
Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Global variables
bool 
  done = false,
  flag = true, 
  init_setflag = false;
byte 
  mode = 0, 
  bright, 
  sat, 
  n, 
  last_n,
  hourset = 0,
  p_index,
  h_index;
int  
  timeset = 0, 
  time_passed, 
  time_running, 
  last_counter = 0;
long 
  hue,
  elem,
  comp,
  tri1,
  tri2,
  scheme[24];
unsigned long 
  time;



// Interrupt variables
volatile byte 
  second = 0;
volatile int 
  minute = 0, 
  counter = 0;
volatile bool 
  press_flag = false, 
  press_flag2 = false, 
  release_flag = false, 
  shortpress = false, 
  longpress = false,
  increment = false,
  decrement = false;
volatile unsigned long 
  press_time, 
  release_time;

volatile unsigned long 
  timer = 0;

union u_conv
{
    long    lValue;
    byte    lByte[4];
        
};

u_conv
    strt,
    finish;

// Internal interrupt for ms
ISR(TIMER1_COMPA_vect) {
    timer++;
    if(timer % 1000 == 0){
      PORTB ^= (1 << 5);
      second++;
    }
}


void setup() {

  Serial.begin(9600);

// Set internal LED to OUTPUT
  DDRB = (1 << 5); 

// See Ben Finio tutorial on YouTube
  TCCR1A = 0;
  TCCR1B |=(1 << WGM12);
  OCR1A = 250;  
  TIMSK1 = 0B00000010;
  sei(); 
   // 64 prescale factor
  TCCR1B |= (1 << CS10)|(1 << CS11); 
  
// set rotary encoder pins as INPUT_PULLUP
  PORTD = 0b00011100;

  attachInterrupt(digitalPinToInterrupt(s2), update, CHANGE);
  attachInterrupt(digitalPinToInterrupt(key), button, CHANGE);

// Read parameters from EEPROM and reconstruct the hue from 4 bytes
// using an union (see union_example.cpp in the examples folder)
  bright = EEPROM.read(1);
  sat = 250;
  for(int i = 3; i < 7; i++){
        finish.lByte[i-3] = EEPROM.read(i);
    }
  hue = finish.lValue;

  
  pixel.begin();
  pixel.show();
  
  colors();
  
}
  void loop() {
    click();
    while(!init_setflag) {
      click();
      
      if (increment) {
        timeset++;
        if (timeset % 60 == 0) {
          hourset++;
        }
        increment = false;
        decrement = false;
      }
      if (decrement && timeset >= 1) {
        if (timeset % 60 == 0) {
          hourset--;
        }
        timeset--;
        increment = false;
        decrement = false;
      }
      int minuteset = timeset - hourset*60;
      switch (minuteset)
      {
      case 0:
        p_index = 0;
        break;
      case 1 ... 14:
        p_index = map(minuteset, 0, 15, 0, 6);
        p_index = exclusive(p_index, 0, 6);
        break;
      case 15:
        p_index = 6;
        break;
      case 16 ... 29:
        p_index = map(minuteset, 15, 30, 6, 12);
        p_index = exclusive(p_index, 6, 12);
        break;
      case 30:
        p_index = 12;
        break;
      case 31 ... 44:
        p_index = map(minuteset, 30, 45, 12, 18);
        p_index = exclusive(p_index, 12, 18);
        break;
      case 45:
        p_index = 18;
        break;
      case 46 ... 59:
        p_index = map(minuteset, 45, 60, 18, 24);
        p_index = exclusive(p_index, 18, 24);
        break;
      }


      h_index = hourset*2 + 2;
      for (int i = 0; i < 24; i++) {
        if (i == p_index) {
          scheme[i] = elem;
        }
        
        else if (i % 2 == 0) {
          if ( i < h_index && i != 0) {
            scheme[i] = tri1;
          } else {
          scheme[i] = tri2;
          }
        }
        else {
          scheme[i] = 0;
        }
        pixel.setPixelColor(i, scheme[i]);
        pixel.show();
      }
      Serial.print("Hour Set: ");
      Serial.print(hourset);
      Serial.print("\tMinute Set: ");
      Serial.print(minuteset);
      Serial.print("\tPixel Index: ");
      Serial.print(p_index);
      Serial.print("\tTime Set: ");
      Serial.println(timeset);

      if (shortpress) {
        init_setflag = true;
        shortpress = false;
        longpress = false; // error-prevention line
        timeset *= 60;
        counter = 0;
        second = 0;
        minute = 0;
        Serial.println("Time set!");
      }
      
    }
  }