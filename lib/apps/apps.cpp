#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <apps.h>
#include <OneButton.h>

extern int mode, counter, second, minute;
extern bool shortpress, longpress;
extern long time, last_time;
extern unsigned long pressStartTime;

extern OneButton Key;

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
  if (time - last_time >= 1000) {
        second++;
        last_time = time;

        Serial.print(minute);
        Serial.print(":");
        Serial.println(second);

        if (second >= 59) {
          second = -1;
          minute++;
          }
        }
}

void checkTicks() {
  Key.tick();
}

// this function will be called when the button was pressed 1 time only.
void singleClick()
{
  shortpress = true;
} // singleClick


// this function will be called when the button was pressed 2 times in a short timeframe.
void doubleClick()
{

} // doubleClick


// this function will be called when the button was pressed multiple times in a short timeframe.
void multiClick()
{
  Serial.print("multiClick(");
  Serial.print(Key.getNumberClicks());
  Serial.println(") detected.");

} // multiClick


// this function will be called when the button was held down for 1 second or more.
void pressStart()
{
  longpress = true;
  Serial.println("pressStart()");
  pressStartTime = millis() - 1000; // as set in setPressTicks()
} // pressStart()


// this function will be called when the button was released after a long hold.
void pressStop()
{
  Serial.print("pressStop(");
  Serial.print(millis() - pressStartTime);
  Serial.println(") detected.");
} // pressStop()