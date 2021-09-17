// Include Files
#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include "apps.h"

#define LED_PIN    A1
#define LED_COUNT 24
#define key 2
#define s2 3
#define s1 4
// Objects declaration
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Global variables
byte mode = 0, counter = 0, last_counter = 0;
byte bright, sat, n, last_n;
int  timeset, time_passed, time_running;
long color, hue;
unsigned long time;
bool done = false;
bool flag = true, init_setflag = false;
// Interrupt variables
volatile byte second = 0;
volatile int minute = 0;
volatile bool press_flag = false, press_flag2 = false;
volatile bool release_flag = false, shortpress = false;
volatile bool longpress = false;
volatile unsigned long press_time, release_time;

volatile unsigned long timer = 0;

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


int main(void) {

  Serial.begin(9600);

// Set internal LED to OUTPUT
  DDRB = (1 << 5); 

// See Ben Finio tutorial on YouTube
  TCCR1B |=(1 << WGM12);
  OCR1A = 0xF9;  
  TIMSK1 = 0B00000010;
  sei(); 
  TCCR1B |=(1 << CS11);
  TCCR1B |=(1 << CS10); 
  
  pinMode (s1, INPUT_PULLUP);
  pinMode (s2, INPUT_PULLUP);
  pinMode(key, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(s2), update, CHANGE);
  attachInterrupt(digitalPinToInterrupt(key), button, CHANGE);

// Read parameters from EEPROM and reconstruct the hue from 4 bytes
// using an union (see union_example.cpp in the examples folder)
  bright = EEPROM.read(1);
  sat = EEPROM.read(2);
  for(int i = 3; i < 7; i++){
        finish.lByte[i-3] = EEPROM.read(i);
    }
  hue = finish.lValue;

  while(1) {
    
    click();

    while(!init_setflag) {
      timeset = counter;
      click();
      Serial.print(timeset);
      Serial.println(" minutes");
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
      
    switch (mode)
    {
      case 1:
        // Reset
        Serial.println("Reset");
        counter = 0;
        mode = 0;
        break;  

      case 2:
        // Brightness setting
          counter = map(bright, 0, 255, 0, 25);
        while(!shortpress) {
          click();
          bright = constrain(map(counter, 0, 25, 0 , 255), 0, 255);
          Serial.print("Brightness: ");
          Serial.println(bright);
        }
        EEPROM.update(1, bright);
        shortpress = false;
        mode++;
        break;

      case 3:
        // Color setting
          counter = map(hue, 0, 65536, 0, 50);
        while (!shortpress) {
          click();
          hue = constrain(map(counter, 0, 50, 0, 65536), 0, 65536);
          Serial.print("Hue value: ");
          Serial.println(hue);
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
          counter = map(sat, 0, 255, 0, 25);
        while (!shortpress) {
          click();
          sat = constrain(map(counter, 0, 25, 0 , 250), 0, 255);
          Serial.print("Saturation: ");
          Serial.println(sat);
        }
        EEPROM.update(2, sat);
        shortpress = false;
        mode++;
        break;
      
      case 5:
        mode = 0;
        break;

      default:
        
        modeSelect();
        color = strip.ColorHSV(hue, sat, bright);
      //Increments time in variables minute and second
        timeKeeper();  
        time_passed = minute*60 + second;
        time_running = timeset - time_passed;
        time_running = constrain(time_running, 0, timeset);
        
      // Sleep http://www.gammon.com.au/power
        if (time_running <= 0) {
          Going_To_Sleep();

        } else {
            n = constrain(map(time_running, 0, timeset, 0, 23), 0, 23);

            if (n != last_n) {
              last_n = n;
              for (int j = 0; j < n; j++) {
                strip.setPixelColor(j, color);
              }
              for (int i = n; i <= 23; i++ ) {
                strip.setPixelColor(i, 0, 0, 0);
              }
            }
        }
    }
  }
}
