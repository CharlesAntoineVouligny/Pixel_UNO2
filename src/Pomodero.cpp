#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include "papps.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define POT         A0
#define PUSH        2
#define NPN         6
#define LED_PIN     8
#define LED_COUNT   24

// Objects declaration
Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Global variables
bool
  focus = true,
  rest = false,
  press = false,
  singlepress = false,
  doublepress = false,
  longpress = false,
  press_flag = false,
  press_flag2 = false,
  release_flag = false,
  update = true;
byte 
  brightness = 5,
  presscount = 0,
  minute = 0,
  second = 0,
  minute_left;
int
  last_pot_reading,
  interval = 250,
  seconds_left;
long
  press_time,
  release_time,
  last_reading_time = 0,
  last_second = 0,
  red,
  blue;

void setup() {
  Serial.begin(9600);

  pinMode(POT, INPUT);
  pinMode(PUSH, INPUT_PULLUP);
  pinMode(NPN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(NPN, HIGH);
  last_pot_reading = analogRead(POT);
  attachInterrupt(digitalPinToInterrupt(PUSH), button, CHANGE);
  pixel.begin();
  red = pixel.ColorHSV(22000, 255, brightness);
  blue = pixel.ColorHSV(11000, 255, brightness);

}

void loop() {
  click();
  if (millis() > last_reading_time + interval) {
    last_reading_time = interval;
    int pot_reading = analogRead(POT);
    if (pot_reading <= last_pot_reading - 4 || 
        pot_reading >= last_pot_reading + 4) {
          last_pot_reading = pot_reading;
          brightness = map(last_pot_reading, 0, 1023, 0, 255);
          red = pixel.ColorHSV(22000, 255, brightness);
          blue = pixel.ColorHSV(11000, 255, brightness);
        }
  }
  
  if (millis() >= last_second + 10) {
    last_second = millis();
    second++;
    update = true;
    if (second > 60) {
      second = 0;
      minute++;
    }
  }

  if (focus) {
    seconds_left = 25*60 - minute*60 - second;

    if (update) {
      byte light = map(seconds_left, 0 , 25*60, 0, 25);
      pixel.fill(red, 0, light);
      pixel.fill(0,light);

      pixel.show();
      update = false;
    }
    if (seconds_left == 0) {
      sleep();
      focus = false;
      rest = true;
      minute = 0;
      second = 0;
    }
  }
  if (rest) {
    seconds_left = (5*60 - minute*60 - second);
    if (update) {
      byte light = map(seconds_left, 0 , 5*60, 0, 25);
      pixel.fill(blue, 0, light);
      pixel.fill(0,light);
      pixel.show();
      update = false;
    }
    if (seconds_left == 0) {
      sleep();
      focus = true;
      rest = false;
      minute = 0;
      second = 0;
    }
  }

}
