#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include <papps.h>

#define POT         A0
#define PUSH        2
#define NPN         6
#define LED_PIN     8
#define LED_COUNT   24

// Extern object
extern Adafruit_NeoPixel pixel;

// Extern Global Variables
extern bool
  press,
  singlepress,
  doublepress,
  longpress,
  press_flag,
  press_flag2,
  release_flag;

extern byte 
  mode,
  brightness,
  presscount;

extern long
  press_time,
  release_time;

// Local Globals



void wakeUp(){
  sleep_disable();
  detachInterrupt(0);
}

void sleep(){
  // make sure NeoPixels are off
  pixel.clear();
  pixel.show();
  delay(10);
// See http://www.gammon.com.au/power or sleep example
    detachInterrupt(digitalPinToInterrupt(PUSH));

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
    pinMode(POT, INPUT);
    pinMode(PUSH, INPUT_PULLUP);
    pinMode(NPN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    digitalWrite(NPN, HIGH);
    attachInterrupt(digitalPinToInterrupt(PUSH), button, CHANGE);
  }

void button() {
    if (digitalRead(PUSH) == LOW) {
    press_flag = true;
  } else {
    release_flag = true;

  }
}

void click() {


  //if this is true the button was just pressed down
    if(press_flag) {
    //note the time the button was pressed
      press_time = millis();
      press_flag = false;
      press_flag2 = true;
    }
    //if its been more than 850ms
    if(millis() - press_time > 850 && press_flag2) {

        longpress = true;
        press_flag2 = false;
        
    }
    //if it's been less
    if(release_flag) {

      release_time = millis();
      release_flag = false;
      
      
      //single click flag
      if((release_time - press_time) > 50 && 
      (release_time - press_time) < 300)
      {
        press_flag = false;
        press_flag2 = false;
        presscount++;
      }
    }
    //After a set delay(300ms is fine), uC sorts double from single clicks
    if (millis() - release_time >= 300) {
      switch (presscount)
      {
      case 1:
        singlepress = true;
        presscount = 0;
        break;
      case 2 ... 5:
        doublepress = true;
        presscount = 0;
        break;

      default:
        presscount = 0;
        break;
      }
    }
    
}