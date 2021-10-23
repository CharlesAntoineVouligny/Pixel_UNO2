#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include<avr/interrupt.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define POT         A3
#define PUSH        2
#define NPN         1
#define LED_PIN     0
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

// Functions

ISR (PCINT0_vect)        // Interrupt service routine 
{
  MCUCR&=~(1<<SE);      //Disabling sleep mode inside interrupt routine
}
void external_interrupt()
{
  sei();                //enabling global interrupt
  GIMSK|= (1<<PCIE);    //Pin change interrupt enable
  PCMSK|=(1<<PCINT2);   //Pin change interrupt to 2nd pin PB2
}
void sleep(){
  // make sure NeoPixels are off
  pixel.clear();
  pixel.show();
  delay(10);

  // Set all pins as outputs except pin 7
  // make sure they're low
    for (byte i = 0; i < 5; i++) {
      if (i != 2) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
      }   
    }
  external_interrupt();
    
  MCUCR|=(1<<SM1);      // enabling sleep mode and powerdown sleep mode
  MCUCR|= (1<<SE);     //Enabling sleep enable bit
  __asm__ __volatile__ ( "sleep" "\n\t" :: ); //Sleep instruction to put controller to sleep
  //controller stops executing instruction after entering sleep mode
      
  // wakey wakey
    
  
    pinMode(POT, INPUT);
    pinMode(PUSH, INPUT_PULLUP);
    pinMode(NPN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    digitalWrite(NPN, HIGH);
    focus = !focus;
    rest = !rest;
    minute = 0;
    second = 0;
  }

void setup() {

  pinMode(POT, INPUT);
  pinMode(PUSH, INPUT_PULLUP);
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
  if (millis() > last_reading_time + interval) {
    last_reading_time = interval;
    int pot_reading = analogRead(POT);
    if (pot_reading <= last_pot_reading - 4 || 
        pot_reading >= last_pot_reading + 4) {
          last_pot_reading = pot_reading;
          brightness = constrain(map(last_pot_reading, 100, 1023, 0, 100), 0 , 100);
          update = true;
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
      red = pixel.ColorHSV(22000, 255, brightness);
      byte light = map(seconds_left, 0 , 25*60, 0, 25);
      pixel.fill(red, 0, light);
      pixel.fill(0,light);

      pixel.show();
      update = false;
    }
    if (seconds_left == 0) {
      sleep();
      
    }
  }
  if (rest) {
    seconds_left = (5*60 - minute*60 - second);
    if (update) {
      blue = pixel.ColorHSV(11000, 255, brightness);
      byte light = map(seconds_left, 0 , 5*60, 0, 25);
      pixel.fill(blue, 0, light);
      pixel.fill(0,light);
      pixel.show();
      update = false;
    }
    if (seconds_left == 0) {
      sleep();
      
    }
  }
}
