// Include Files
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "apps.h"

// Objects declaration
#define LED_PIN    A1
#define LED_COUNT 24
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

extern int key, s1, s2;

// Global variables
int mode = 0, counter = 0, last_counter = 0, timeset;
volatile int second = 0, minute = 0;
int bright, sat, time_passed, time_running, n, last_n;
int last_second, time_mod;
long color, hue;
unsigned long time, last_time = 0;
bool shortpress = false, longpress = false, done = false;
bool flag = true, init_setflag = false, flag2 = true;
volatile bool press_flag = false;

// Timer/Counter1 Compare Match A
// Internal interrupt to count time
ISR(TIMER1_COMPA_vect) {
    // Increment time
    PORTB ^= 0b00100000; //Toggles intern LED (pin13)
    second++;
}

void click() {
  press_flag = true;
}


int main(void) {

  Serial.begin(9600);


  DDRB = 0b00100000; // Set internal LED to OUTPUT

  TCCR1B = 0b00001101; // CTC mode enabled WGM12 (xxxx1xxx)  and prescaler set to 1024bits (xxxxx101)
  // Set timer limit (OCR1A/B + 1) * prescaler * 62.5 nanoSeconds
    //(15624 + 1) * 1024 * 62.5 x10^-9 = 1 second
    OCR1A = 15624; // Clear Timer on Compare mode(CTC) maximum value 
    TIMSK1 = 0B00000010; // Set OCIEA bit true to enable compare with OCR1A 
    // Output compare A match interrupt enable
    
  pinMode (s1, INPUT);
  pinMode (s2, INPUT);
  pinMode(key, INPUT);

  attachInterrupt(digitalPinToInterrupt(s2), update, CHANGE);
  attachInterrupt(digitalPinToInterrupt(key), click, CHANGE);

  sei(); //enable interrupts globally



  while (1) {
    
    button();
    if (shortpress) {
      Serial.println("Short Press!");
      shortpress = !shortpress;
    }
    if (longpress) {
      Serial.println("Long Press!");
      longpress = !longpress;
    }
    // while(!init_setflag) {
    //   timeset = counter;
    //   Serial.print(timeset);
    //   Serial.println(" minutes");
    //   if (shortpress) {
    //     init_setflag = true;
    //     shortpress = false;
    //     timeset *= 60;
    //     counter = 0;
    //     second = 0;
    //     minute = 0;
    //     Serial.println("Time set!");
    //   }
    // }
    
    // switch (mode)
    // {
    //   case 1:
    //     // Reset
    //     Serial.println("Reset");
    //     counter = 0;
    //     mode = 0;
    //     flag = true;
    //     break;  

    //   case 2:
    //     // Brightness setting
        
    //     while(!shortpress) {
    //       bright = constrain(map(counter, 0, 25, 0 , 250), 0, 250);
    //       Serial.print("Brightness: ");
    //       Serial.println(bright);
    //     }
    //     shortpress = false;
    //     mode++;
    //     break;

    //   case 3:
    //     // Color setting
    //     while (!shortpress) {
    //       hue = constrain(map(counter, 0, 50, 0, 65536), 0, 65536);
    //       Serial.print("Hue value: ");
    //       Serial.println(hue);

    //     }
    //     shortpress = false;
    //     mode++;
    //     break;
      
    //   case 4:
    //     // Saturation setting
    //     while (!shortpress) {
    //     sat = constrain(map(counter, 0, 25, 0 , 250), 0, 250);
    //       Serial.print("Saturation: ");
    //       Serial.println(sat);
    //       

    //     }
    //     shortpress = false;
    //     mode++;
    //     break;
      
    //   case 5:
    //     mode = 0;
    //     break;

    //   default:
        
    //     color = strip.ColorHSV(hue, sat, bright);
    //     button(); // 1 press is reset, longpress is parameters
    //     // timeKeeper();  //Increments time in variables minute and second

    //     time_passed = minute*60 + second;
    //     time_running = timeset - time_passed;
    //     time_running = constrain(time_running, 0, timeset);
        
    //     if (time_running <= 0 && !done) {
    //       Serial.println("Done!");
    //       done = true;

    //     } else {
        
            
    //         n = constrain(map(time_running, 0, timeset, 0, 23), 0, 23);
    //         if (n != last_n) {
    //           last_n = n;
    //           for (int j = 0; j < n; j++) {
    //             strip.setPixelColor(j, color);
    //             Serial.print(j);
    //             Serial.println(" leds on");
    //           }
    //           for (int i = n; i <= 23; i++ ) {
    //             strip.setPixelColor(i, 0, 0, 0);
    //             Serial.print(i);
    //             Serial.println(" leds off");
    //           }
    //         }
    //     }
    // }
  }
}