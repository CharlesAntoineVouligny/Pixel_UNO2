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
  p_index;
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
      }
      if (decrement && timeset != 0) {
        timeset--;
        if (timeset % 60 == 0) {
          hourset--;
        }
        
        decrement = false;
      }
      int minuteset = timeset - hourset*60;
      minuteset = map(minuteset, 0, 60, 0 ,24);
      
      for (int i = 0; i < 24; i++) {
        if (i == minuteset) {
          scheme[i] = elem;
        }
        else if ( i < hourset ) {
          scheme[i] = tri1;
        }
        else if (i == 0 || i == 6 || i == 12 || i == 18) {
          scheme[i] = tri2;
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