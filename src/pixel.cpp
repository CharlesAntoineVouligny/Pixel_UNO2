#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <OneButton.h>
#include "apps.h"

#define LED_PIN    A1
#define LED_COUNT 24
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

extern int key, s1, s2;
OneButton Key(key, true);



int mode = 0, counter = 0, last_counter = 0, timeset, second = -1;
int bright, sat, minute = 0, time_passed, time_running, n, last_n;
int last_second, time_mod;
long color, hue;
unsigned long time, last_time = 0, pressStartTime;
bool shortpress = false, longpress = false, done = false;

bool flag = true, init_setflag = false, flag2 = true;



void setup() {

  Serial.begin(9600);

  pinMode (s1, INPUT);
  pinMode (s2, INPUT);

  attachInterrupt(digitalPinToInterrupt(s2), update, CHANGE);
   attachInterrupt(digitalPinToInterrupt(key), checkTicks, CHANGE);

  // link the xxxclick functions to be called on xxxclick event.
  Key.attachClick(singleClick);
  Key.attachDoubleClick(doubleClick);
  Key.attachMultiClick(multiClick);

  Key.setPressTicks(1000); // that is the time when LongPressStart is called
  Key.attachLongPressStart(pressStart);
  Key.attachLongPressStop(pressStop);
}

void loop() {
  Key.tick();
  
  while(!init_setflag) {
    timeset = counter;
    Serial.print(timeset);
    Serial.println(" minutes");
    Key.tick();
    if (shortpress) {
      init_setflag = true;
      shortpress = false;
      timeset *= 60;
      counter = 0;
      Serial.println("Time set!");
    }
  }
  
  switch (mode)
  {
    case 1:
      // Reset
      Serial.println("Reset");
      counter = 0;
      mode = 0;
      flag = true;
      break;  

    case 2:
      // Brightness setting
      
      while(!shortpress) {
        bright = constrain(map(counter, 0, 25, 0 , 250), 0, 250);
        Serial.print("Brightness: ");
        Serial.println(bright);
        Key.tick();
      }
      shortpress = false;
      mode++;
      break;

    case 3:
      // Color setting
      while (!shortpress) {
        hue = constrain(map(counter, 0, 50, 0, 65536), 0, 65536);
        Serial.print("Hue value: ");
        Serial.println(hue);
        Key.tick();

      }
      shortpress = false;
      mode++;
      break;
    
    case 4:
      // Saturation setting
      while (!shortpress) {
      sat = constrain(map(counter, 0, 25, 0 , 250), 0, 250);
        Serial.print("Saturation: ");
        Serial.println(sat);
        Key.tick();

      }
      shortpress = false;
      mode++;
      break;
    
    case 5:
      mode = 0;
      break;

    default:
      
      color = strip.ColorHSV(hue, sat, bright);
      time = millis();
      button();
      timeKeeper();  //Increments time in variables minute and second

      time_passed = minute*60 + second;
      time_running = timeset - time_passed;
      time_running = constrain(time_running, 0, timeset);
      
      if (time_running <= 0 && !done) {
        Serial.println("Done!");
        done = true;

      } else {
      
          
          n = constrain(map(time_running, 0, timeset, 0, 23), 0, 23);
          if (n != last_n) {
            last_n = n;
            for (int j = 0; j < n; j++) {
              strip.setPixelColor(j, color);
              Serial.print(j);
              Serial.println(" leds on");
            }
            for (int i = n; i <= 23; i++ ) {
              strip.setPixelColor(i, 0, 0, 0);
              Serial.print(i);
              Serial.println(" leds off");
            }
          }
      }
  }
}