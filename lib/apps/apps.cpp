#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include <apps.h>


// Extern variables
extern volatile int second, minute;
extern volatile bool press_flag, press_flag2, release_flag, shortpress, longpress;
extern volatile unsigned long press_time, release_time, timer;



extern int mode, counter;
extern unsigned long time, last_time;
extern bool init_setflag;

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

// Interrupts

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

void wakeUp(){
  Serial.println("Interrrupt Fired");//Print message to serial monitor
  sleep_disable();//Disable sleep mode
  detachInterrupt(0); //Removes the interrupt from pin 2;
}

void Going_To_Sleep(){
    detachInterrupt(digitalPinToInterrupt(s2));
    detachInterrupt(digitalPinToInterrupt(key));
    for (byte i = 0; i < 20; i++) {
      if(i != 2) {
        pinMode(i, OUTPUT);
      }
    }
    // disable ADC
    ADCSRA = 0;
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); //full sleep, only wakeable from int or wdt
    sleep_enable();//Enabling sleep mode
    // Do not interrupt before we go to sleep, or the
    // ISR will detach interrupts and we won't wake.
    noInterrupts();
    attachInterrupt(0, wakeUp, LOW);
    EIFR = bit (INTF0);
    // turn off brown-out enable in software
    // BODS must be set to one and BODSE must be set to zero within four clock cycles
    MCUCR = bit (BODS) | bit (BODSE);
    // The BODS bit is automatically cleared after three clock cycles
    MCUCR = bit (BODS); 
    
    // We are guaranteed that the sleep_cpu call will be done
    // as the processor executes the next instruction after
    // interrupts are turned on.
    interrupts ();  // one cycle
    sleep_cpu ();   // one cycle
    pinMode(3, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(s2), update, CHANGE);
    attachInterrupt(digitalPinToInterrupt(key), button, CHANGE);
    counter = 0;
    init_setflag = false;
  }