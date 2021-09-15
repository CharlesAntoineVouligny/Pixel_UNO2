#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <apps.h>
#include <OneButton.h>

// Extern variables
extern int mode, counter; 
extern volatile int second, minute;
extern bool shortpress, longpress;
extern long time, last_time;
extern unsigned long pressStartTime;
extern OneButton Key;

// Local global variables
int key = 2, s1 = 4, s2 = 3; // Rotary Encoder Pins
int pinAstateCurrent = LOW;        // Current state of Pin A
int pinAStateLast = pinAstateCurrent;

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

void shortPress() {
    shortpress = true;
}

void longPress() {
    longpress = true;
}

void button() {
  // Button --> 1 press to reset counter
      if (shortpress) {
        mode++;
        shortpress = false;
      }
      // Button --> Hold 1 sec to access parameters
      if (longpress) {
        mode = 2;
        longpress = false;
      }
}

void timeKeeper() {
  if(second > 60) {
    second = 0;
    minute++;
  }
}

void click() {
  shortpress = true;
  Serial.println(shortpress);
}