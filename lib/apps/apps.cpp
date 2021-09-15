#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <apps.h>

// Extern variables
extern volatile int second, minute;
extern volatile bool press_flag;

unsigned long press_time, release_time;

extern int mode, counter;
extern long time, last_time;
extern bool shortpress, longpress;

// Local global variables
int key = 2, s1 = 4, s2 = 3; // Rotary Encoder Pins
int pinAstateCurrent = LOW;        // Current state of Pin A
int pinAStateLast = pinAstateCurrent;
int press = 0;

bool second_press = false;

void update() {

  // ROTATION DIRECTION
  pinAstateCurrent = digitalRead(s1);
  
  // If there is a minimal movement of 1 step
  if ((pinAStateLast == LOW) && (pinAstateCurrent == HIGH)) {

    if (digitalRead(s2) == HIGH) { 
      counter++;
      Serial.println(counter);             // Print on screen
    } else {
      counter--;
      Serial.println(counter);            // Print on screen
    }
    
  }

  pinAStateLast = pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}

void timeKeeper() {
  if(second > 60) {
    second = 0;
    minute++;
  }
}

void button() {
  
  if (press_flag) {
    press_flag = !press_flag;
    
    if (second_press) {
      second_press = !second_press;
      release_time = millis();

      if (release_time - press_time >= 1000) {
        longpress = true;
      } 
      else {
        shortpress = true;
      }

    } 
    else {
      second_press = true;
      press_time = millis();
    }

    
  } 
}

