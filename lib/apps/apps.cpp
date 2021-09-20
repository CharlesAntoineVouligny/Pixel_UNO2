#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include <apps.h>

#define LED_PIN    A1
#define LED_COUNT 24
#define key 2
#define s2 3
#define s1 4

// Extern object
extern Adafruit_NeoPixel pixel;

// Extern interrupt variables
extern volatile byte 
  second;
extern volatile int 
  minute, 
  counter;
extern volatile bool 
  press_flag, 
  press_flag2, 
  release_flag;
extern volatile bool 
  shortpress, 
  longpress,
  increment,
  decrement;
extern volatile unsigned long 
  press_time, 
  release_time, 
  timer;

// Extern globals
extern byte 
  mode, 
  bright, 
  sat;
extern unsigned long 
  time;
extern bool 
  init_setflag;
extern long
  hue,
  elem,
  comp,
  tri1,
  tri2,
  scheme[24];
// Local global
int pinAstateCurrent = LOW;
int pinAStateLast = pinAstateCurrent;
int press = 0;


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

// Interrupts

void update() {

  // ROTATION DIRECTION
  pinAstateCurrent = digitalRead(s1);
  
  // If there is a minimal movement of 1 step
  if ((pinAStateLast == LOW) && (pinAstateCurrent == HIGH)) {

    if (digitalRead(s2) == HIGH) { 
      counter++;
      increment = true;
    } else {
      counter--;
      decrement = true;
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
    //if its been more than 850ms
    else if(timer - press_time > 850 && press_flag2) {

        longpress = true;
        press_flag2 = false;
        
    }
    //if it's been less
    else if(release_flag) {

      release_time = timer;
      release_flag = false;
      
      if((release_time - press_time) > 50 && (release_time - press_time) < 800)
      {
        shortpress = true;
        press_flag = false;
        press_flag2 = false;
      }
    }
}

// SLEEP

void wakeUp(){
  sleep_disable();
  detachInterrupt(0);
}

void Going_To_Sleep(){
// See http://www.gammon.com.au/power or sleep example
    detachInterrupt(digitalPinToInterrupt(s2));
    detachInterrupt(digitalPinToInterrupt(key));

  // Set all pins as outputs except pin 2
    DDRB = 0b00111111;
    DDRC = 0b00111111;
    DDRD = 0b11111011;
  // make sure they're low
    PORTB &= 0b11000000;
    PORTC &= 0b11000000;
    PORTD = 0b00000100;
    
    ADCSRA = 0;
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
    sleep_enable();
    noInterrupts();
    attachInterrupt(0, wakeUp, LOW);
    EIFR = bit (INTF0);
    MCUCR = bit (BODS) | bit (BODSE);
    MCUCR = bit (BODS); 
    interrupts (); 
    sleep_cpu ();  
  // wakey wakey
    DDRD = 0b11100011;
    PORTD = 0b00011100;
    attachInterrupt(digitalPinToInterrupt(s2), update, CHANGE);
    attachInterrupt(digitalPinToInterrupt(key), button, CHANGE);
    counter = 0;
    init_setflag = false;
  }

  // MISC

  void colors() {
    comp = hue + 32768;
    if (comp > pow(2,16)) {
      comp -= pow(2,16);
    }
    tri1 = hue + pow(2,16) / 3;
    if (tri1 > pow(2,16)) {
      tri1 -= pow(2,16);
    }
    tri2 = hue - pow(2,16) / 3;
    if (tri2 < 0) {
      tri2 += pow(2,16);
    }
    elem = pixel.ColorHSV(hue, sat, bright);
    comp = pixel.ColorHSV(comp, sat, bright);
    tri1 = pixel.ColorHSV(tri1, sat, bright);
    tri2 = pixel.ColorHSV(tri2, sat, bright);
  }

  int roundCounter(int max) {
      if (counter > max) {
        counter = 0;
      } else if (counter < 0) {
        counter = max;
    }
    return counter;
  }

  void clockArray() {
  for (int i = 0; i < 24; i++) {
    if (i == 0 || i == 6 || i == 12 || i == 18) {
      scheme[i] = comp;
    } else {
      scheme[i] = elem;
    }
  }
  }