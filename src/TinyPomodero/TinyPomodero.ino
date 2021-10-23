#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define POT         A3
#define PUSH        2
#define NPN         0
#define LED_PIN     1
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
  j = 0,
  last_light = 0,
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

void sleep() {
    for (byte i = 0; i < 5; i++)   {
      if (i != 2) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
      }
    }
    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT2);                   // Use PB2 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT2);                  // Turn off PB2 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on

    sei();                                  // Enable interrupts
    pinMode(POT, INPUT);
    pinMode(PUSH, INPUT);
    digitalWrite(NPN, HIGH);
    } // sleep

ISR(PCINT0_vect) {
    // This is called when the interrupt occurs, but I don't need to do anything in it
    }

void setup() {
  for (byte i = 0; i < 5; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i,LOW);
  }
  pinMode(POT, INPUT);
  pinMode(PUSH, INPUT);
  pinMode(NPN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(NPN, HIGH);
  last_pot_reading = analogRead(POT);
  brightness = constrain(map(last_pot_reading, 100, 1023, 0, 100), 0, 100);
  pixel.begin();
  red = pixel.ColorHSV(22000, 255, brightness);
  blue = pixel.ColorHSV(11000, 255, brightness);

}

void loop() {

  int pot_reading = analogRead(POT);

  if (pot_reading <= last_pot_reading - 4 || 
      pot_reading >= last_pot_reading + 4) 
    {
    last_pot_reading = pot_reading;
    brightness = constrain(map(last_pot_reading, 100, 1023, 0, 100), 0 , 100);
    unsigned int
      redColor = 22000 - j*200,
      blueColor = 25000 + j*1000;
    red = pixel.ColorHSV(redColor, 255, brightness);
    blue = pixel.ColorHSV(blueColor, 255, brightness);
    update = true;
    }
  if (millis() >= last_second + 10) {
    last_second = millis();
    second++;
    if (second > 60) {
      second = 0;
      minute++;
    }
    if (second % 15 == 0) {
      j++;
    }
  }

  if (focus) {
    seconds_left = 25*60 - minute*60 - second;
    byte light = map(seconds_left, 0 , 25*60, 0, 25);
    if (last_light != light) {
      last_light = light;
      unsigned int color = 22000 - j*200;
      red = pixel.ColorHSV(color, 255, brightness);
      
      update = true;
    }

    if (update) {
      
      pixel.fill(red, 0, last_light);
      pixel.fill(0,last_light);
      pixel.show();
      update = false;
        
      
    }
    if (seconds_left == 0) {
      sleep();
      focus = false;
      rest = true;
      minute = 0;
      second = 0;
      j = 0;
    }
  }
  
  if (rest) {
    seconds_left = 5*60 - minute*60 - second;
    byte light = map(seconds_left, 0 , 5*60, 0, 25);
    if (last_light != light) {
      last_light = light;
      unsigned int color = 25000 + j*1000;
      blue = pixel.ColorHSV(color, 255, brightness);
      
      update = true;
    }

    if (update) {
      
      pixel.fill(blue, 0, last_light);
      pixel.fill(0,last_light);
      pixel.show();
      update = false;
        
      
    }
    if (seconds_left == 0) {
      sleep();
      focus = true;
      rest = false;
      minute = 0;
      second = 0;
      j = 0;
    }
  }
}
