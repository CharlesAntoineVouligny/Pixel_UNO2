#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include "apps.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_PIN     8
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
  init_setflag = false,
  setting = false,
  refresh = false;
byte 
  mode = 0, 
  bright, 
  sat, 
  n, 
  last_n,
  hourset = 0,
  p_index,
  h_index,
  view_style = false;
int  
  timeset = 0, 
  time_passed, 
  time_running, 
  last_counter = 0,
  presscount = 0;
long 
  hue,
  elem,
  comp,
  tri1,
  tri2,
  scheme[24];
unsigned long 
  time,
  last_timer = 0;



// Interrupt variables
volatile byte 
  second = 0;
volatile int 
  minute = 0, 
  counter = 0;
volatile bool 
  press_flag = false, 
  press_flag2 = false,
  press_flag3 = false, 
  release_flag = false, 
  shortpress = false,
  doublepress = false, 
  longpress = false,
  increment = false,
  decrement = false;
volatile unsigned long 
  press_time, 
  release_time,
  first_time;

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
  DDRD = (1 << 6);

// See Ben Finio tutorial on YouTube
  TCCR1A = 0;
  TCCR1B |=(1 << WGM12);
  OCR1A = 250;  
  TIMSK1 = 0B00000010;
  sei(); 
   // 64 prescale factor
  TCCR1B |= (1 << CS10)|(1 << CS11); 
  
// set rotary encoder pins as INPUT_PULLUP and enable neopixel
  PORTD = 0b01011100;

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
  
  // Calculate triadic colors based on color setting
  colors();
  
}
  void loop() {
    click();
    timeSet();
    modeSelect();
    
    switch (mode)
    {
      case 1:
         
        // Swap between watch and continuous view styles
      click();
       if (!shortpress) {
        clockStyle();
        } else {
          shortpress = false;
          refresh = true;
          mode = 0;
        }
        
        break;  

      case 2:
        // Brightness setting
        counter = map(bright, 0, 255, 0, 50);
        refresh = true;
          while(!shortpress) {
            click();
            bright = constrain(map(counter, 0, 50, 0 , 255), 0, 255);
            settingDisplay();
          }

        if (bright != 0) {
        EEPROM.update(1, bright);
        }

        shortpress = false;
        mode++;
        break;

      case 3:
        // Color setting
         
        refresh = true;
        counter = map(hue, 0, 65536, 0, 50);
        while (!shortpress) {
          click();
         

          hue = constrain(map(counter, 0, 50, 0, 65536), 0, 65536);
         
          settingDisplay();
        }
        for (int i = 3; i < 7; i++){
        finish.lByte[i-3] = EEPROM.read(i);
        }
        if (hue != finish.lValue) {
          strt.lValue = hue;
          for (int i = 3; i < 7; i++) {
            EEPROM.write(i, strt.lByte[i-3]);
          }
        }
        shortpress = false;
        mode++;
        break;
      
      case 4:
         

        // Saturation setting
        refresh = true;
        counter = map(sat, 0, 255, 0, 25);
        while (!shortpress) {
          click();
          sat = constrain(map(counter, 0, 25, 0 , 250), 0, 255);
          settingDisplay();
        }
        EEPROM.update(2, sat);
        refresh = true;
        shortpress = false;
        mode++;
        break;
      
      case 5:
         

        mode = 0;
        break;

      default:
         
        click();
        display();
        if (shortpress) {
          shortpress = false;
          mode++;
        }
        modeSelect();
      //Increments time in variables minute and second
        timeKeeper();  
        time_passed = minute*60 + second;
        time_running = timeset - time_passed;
        time_running = constrain(time_running, 0, timeset);
      // Sleep http://www.gammon.com.au/power
        if (time_running <= 0) {
          Going_To_Sleep();
        }
    }
  }