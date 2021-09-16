#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <apps.h>

// Extern variables
extern volatile int second, minute;
extern volatile bool press_flag, press_flag2, release_flag, shortpress, longpress;
extern volatile unsigned long press_time, release_time, timer;



extern int mode, counter;
extern unsigned long time, last_time;

// Local global variables
int key = 2, s1 = 4, s2 = 3; // Rotary Encoder Pins
int pinAstateCurrent = LOW;        // Current state of Pin A
int pinAStateLast = pinAstateCurrent;
int press = 0;

bool second_press = false;


void timeKeeper() {
  if(second > 59) {
    second = 0;
    minute++;
  }
}

void button() {
  if (digitalRead(key) == LOW) {
    press_flag = true;
  } else {
    release_flag = true;
  }
}

int modeSelect() {
  if (shortpress) {
    mode++;
    shortpress = false;
  }
  else if (longpress) {
    mode = 2;
    longpress = false;
  }

  return mode;
}


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


void click() {
  //if this is true the button was just pressed down
    if(press_flag) {

      //note the time the button was pressed
      press_time = timer;
      press_flag = false;
      press_flag2 = true;
      PORTD ^= (1 << 7);
    }
    else if(timer - press_time > 850 && press_flag2) {

        longpress = true;
        press_flag2 = false;
        // Serial.println("Long Press!");
        PORTD ^= (1 << 7);
    }
    //if no button is high one had to be released. The millis function will increase while a button is hold down the loop function will be cycled (no change, so no interrupt is active) 
    else if(release_flag) {

      release_time = timer;
      release_flag = false;
      
      if((release_time - press_time) > 50 && (release_time - press_time) < 800)
      {
        shortpress = true;
        press_flag = false;
        press_flag2 = false;
        // Serial.println("Short Press!");
        PORTD ^= (1 << 7);
      }
    }
}
