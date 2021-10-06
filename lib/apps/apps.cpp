#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include <apps.h>

#define LED_PIN    8
#define LED_COUNT 24
#define key 2
#define s2 3
#define s1 4

// Extern object
extern Adafruit_NeoPixel pixel;

// Extern globals
extern bool 
  init_setflag,
  setting,
  view_style,
  refresh,
  press_flag2, 
  shortpress,
  doublepress, 
  longpress,
  increment,
  decrement;

extern byte 
  mode, 
  bright, 
  sat,
  hourset,
  p_index,
  h_index,
  n,
  last_n,
  last_counter;

extern int  
  timeset,
  time_running,
  time_passed,
  presscount;

extern long
  hue,
  elem,
  comp,
  tri1,
  tri2,
  scheme[24],
  s_scheme[24],
  last_timer;
extern unsigned long
  press_time, 
  release_time;

// Extern interrupt variables
extern volatile byte  
  second,
  counter;
extern volatile int  
  minute;
extern volatile bool 
  press_flag, 
  release_flag,
  pinAstateCurrent,
  pinAStateLast;
extern volatile unsigned long 
  
  timer;

// Local global
 
int press = 0;

bool
  first = true;
byte
  last_second = 0,
  s_index = 23,
  update_ms = 25;
int
  hour2,
  sec2,
  min2;


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

void modeSelect() {
  
  if(doublepress) {
    doublepress = false;
    mode = 2;
  }
  else if (longpress) {
    longpress = false;
    // Reset
    
    init_setflag = false;
    for (byte i = 24; i > 0; i--) {
      pixel.setPixelColor(i, 0);
      pixel.show();
      last_timer = timer;
      while (timer < last_timer + 25) {}
    }
  }
}

// Interrupts

void update() {

  // ROTATION DIRECTION
  pinAstateCurrent = digitalRead(s1);
  
  // If there is a minimal movement of 1 step
  if ((pinAStateLast == LOW) && (pinAstateCurrent == HIGH)) {

    if (digitalRead(s2) == HIGH) { 
      // counter++;
      increment = true;
    } else {
      // counter--;
      decrement = true;
    }
    
  }

  pinAStateLast = pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}

byte count(bool round, byte min, byte max) {
  switch (round)
  {
  case false:
    // Increase/Decrease counter if within range
    if (counter > min && counter < max) {
      if (increment) {
        increment = false;
        counter++;
      }
      else if (decrement) {
        decrement = false;
        counter--;
      }
    }
    else if (increment && counter == min) {
        increment = false;
        counter++;
      }
    else if (decrement && counter == max) {
      decrement = false;
      counter--;
    }
    else {
      increment = false;
      decrement = false;
    }
    break;
  
    
  case true:
    // Round Count
    if (increment) {
      increment = false;
      counter++;
    }
    else if (decrement) {
      decrement = false;
      counter--;
    }
    if (counter >= max) {
      counter = min;
    }
    else if (counter <= min) {
      counter = max;
    }
    
    break;
  }
  return counter;
}

void click() {
  //if this is true the button was just pressed down
    if(press_flag) {

    //note the time the button was pressed
      press_time = timer;
      press_flag = false;
      press_flag2 = true;
    }
    //if its been more than 850ms
    if(timer - press_time > 850 && press_flag2) {

        longpress = true;
        
        press_flag2 = false;
        
    }
    //if it's been less
    if(release_flag) {

      release_time = timer;
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
    if (timer - release_time >= 300) {
      switch (presscount)
      {
      case 1:
        shortpress = true;
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

// SLEEP

void wakeUp(){
  sleep_disable();
  detachInterrupt(0);
}

void Going_To_Sleep(){
  // make sure NeoPixels are off
  pixel.clear();
  pixel.show();
  last_timer = timer;
  while (timer < last_timer + 10) {}
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
    PORTD = 0b01011100;
    attachInterrupt(digitalPinToInterrupt(s2), update, CHANGE);
    attachInterrupt(digitalPinToInterrupt(key), button, CHANGE);
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

  void clockArray() {
  for (byte i = 0; i < 24; i++) {
    if (i == 0 || i == 6 || i == 12 || i == 18) {
      scheme[i] = comp;
    } else {
      scheme[i] = elem;
    }
  }
  }

  int exclusive(byte p_index, byte min, byte max) {
    if (p_index == min) {
          p_index++;
        }
        else if (p_index == max) {
          p_index--;
        }
    return p_index;
  }

  void timeSet() {
    
    while(!init_setflag) {
      click();
      // if it's the first time through the loop, make sure
      // everything's at 0
      if (first) {
        first = !first;
        timeset = 0;
        hourset = 0;
        // Animation
        for(byte i = 0; i < 24; i++) {
          if (i == 0) {
            pixel.setPixelColor(i, tri2);
          }
          else if (i % 2 == 0) {
            pixel.setPixelColor(i, elem);
          }
          pixel.show();
          last_timer = timer;
          while (timer < last_timer + 25) {}
        }
        increment = false;
      }

      // Setting time from rotary encoder interrupts
      if (increment) {
        timeset++;
        if (timeset % 60 == 0) {
          hourset++;
        }
        increment = false;
        decrement = false;
      }
      if (decrement && timeset >= 1) {
        if (timeset % 60 == 0) {
          hourset--;
        }
        timeset--;
        increment = false;
        decrement = false;
      }
      int minuteset = timeset - hourset*60;
      // Live display
      switch (minuteset)
      {
      case 0:
        p_index = 0;
        break;
      case 1 ... 14:
        p_index = map(minuteset, 0, 15, 0, 6);
        p_index = exclusive(p_index, 0, 6);
        break;
      case 15:
        p_index = 6;
        break;
      case 16 ... 29:
        p_index = map(minuteset, 15, 30, 6, 12);
        p_index = exclusive(p_index, 6, 12);
        break;
      case 30:
        p_index = 12;
        break;
      case 31 ... 44:
        p_index = map(minuteset, 30, 45, 12, 18);
        p_index = exclusive(p_index, 12, 18);
        break;
      case 45:
        p_index = 18;
        break;
      case 46 ... 59:
        p_index = map(minuteset, 45, 60, 18, 24);
        p_index = exclusive(p_index, 18, 24);
        break;
      }


      h_index = hourset*2 + 2;
      for (byte i = 0; i < 24; i++) {
        if (i == p_index) {
          scheme[i] = tri2;
        }
        
        else if (i % 2 == 0) {
          if ( i < h_index && i != 0) {
            scheme[i] = tri1;
          } else {
          scheme[i] = elem;
          }
        }
        else {
          scheme[i] = 0;
        }
        pixel.setPixelColor(i, scheme[i]);
        pixel.show();
      }

      if (shortpress || doublepress || longpress) {
        init_setflag = true;
        first = true;
        shortpress = false;
        doublepress = false;
        longpress = false; // error-prevention line
        timeset *= 60;
        counter = 0;
        second = 0;
        minute = 0;
        mode = 0;
        time_running = 0;
        last_n = 0;
        view_style = false;
        
        byte anim[2][24] = {
          {0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0}
        };
        if (p_index != 0) {
          for (byte i = 0; i < 24; i++) {
            
            if (i < p_index) {
              anim[1][i] = p_index - i;
            } 
            if (p_index + i < 24) {
              anim[0][i] = p_index +i;
            }
          }

          for (byte i = 0; i < 24; i++) {
            pixel.setPixelColor(p_index, tri2);
            pixel.show();
            pixel.setPixelColor(anim[0][i],elem);
            pixel.show();
            pixel.setPixelColor(anim[1][i], elem);
            pixel.show();
            last_timer = timer;
            while (timer < last_timer + 25) {}
          }
        }
      }
    } 
  }

  void settingDisplay() {
    switch (mode)
    {
    case 2:
      while (refresh) {
        /* If refresh is true, mode has just been incremented.
         The following is a transition animation:
         Every 25 ms, add 1 pixel in setting style display.
         */
        if (last_timer + update_ms <= timer) {
          last_timer = timer;
          static byte i = 0;
            switch (i)
            {
            case 0 ... 7:
              pixel.setPixelColor(i, elem);
              pixel.show();
              i++;
              break;
            case 8 ... 15:
              pixel.setPixelColor(i, tri1);
              pixel.show();
              i++;
              break;
            case 16 ... 23:
              pixel.setPixelColor(i, tri2);
              pixel.show();
              if (i == 23) {
                i = 0;
                refresh = false;
                break;
              }
              i++;
              break;
            }
        }
      }
      counter = map(bright, 0, 255, 0, 100);
      while (!shortpress) {
        /* 
          This function updates color scheme displayed 
        every time a pulse from the rotary encoder is 
        recorded. Press the rotary encoder button to save
        parameter.
        */
        click();
        bright = constrain(map(count(false, 0, 100), 0, 100, 0 , 255), 5, 255);
        colors();
        if (last_counter != counter || last_timer + update_ms <= timer) {
          last_counter = counter;
          last_timer = timer;
          for (byte i = 0; i < 24; i++) {
            switch (i)
            {
            case 0 ...7:
              pixel.setPixelColor(i, elem);
              pixel.show();
              break;
            case 8 ...15:
              pixel.setPixelColor(i, tri1);
              pixel.show();
              break;
            case 16 ...23:
              pixel.setPixelColor(i, tri2);
              pixel.show();
              break;
            }
          }
        }
      }
      break;
    case 3:
    colors();
      while (refresh) {
        // This transition shifts all colors 120°.
          for (byte j = 0; j <= 8; j++) {

          for (byte i = 0; i < 24; i++) {
            s_scheme[i] = tri2;
          }

          for (byte k = j; k < j + 8; k++) {
            if (k > 23) {
              s_scheme[k - 24] = elem;
            } else {
              s_scheme[k] = elem;
            }
          }
          for (byte l = j + 8; l < j + 16; l++) {
            if (l > 23) {
              s_scheme[l - 24] = tri1;
            } else {
              s_scheme[l] = tri1;
            }
          }
            for (byte i = 0; i < 24; i++) {
              
              pixel.setPixelColor(i, s_scheme[i]);
              pixel.show();
            }
          if (j == 8) {
            refresh = false;
          }
        }
      }
      counter = map(hue, 0, 65536, 0, 200);
      while (!shortpress) {
        /* 
          This function updates color scheme displayed 
        every time a pulse from the rotary encoder is 
        recorded. Press the rotary encoder button to save
        parameter.
        */
        click();
        hue = constrain(map(count(true, 0, 200), 0, 200, 0, 65536), 0, 65536);
        colors();
        if (last_counter != counter || last_timer + update_ms <= timer) {
          last_counter = counter;
          last_timer = timer;
          for (byte i = 0; i < 24; i++) {
            switch (i)
            {
            case 0 ...7:
              pixel.setPixelColor(i, tri2);
              pixel.show();
              break;
            case 8 ...15:
              pixel.setPixelColor(i, elem);
              pixel.show();
              break;
            case 16 ...23:
              pixel.setPixelColor(i, tri1);
              pixel.show();
              break;
            }
          }
        }
      }
      break;
    
    case 4:
      colors();
      while (refresh) {
        // This transition shifts all colors 120°.
          for (byte j = 0; j <= 8; j++) {

          for (byte i = 0; i < 24; i++) {
            s_scheme[i] = tri1;
          }

          for (byte k = j; k < j + 8; k++) {
            if (k > 23) {
              s_scheme[k - 24] = tri2;
            } else {
              s_scheme[k] = tri2;
            }
          }
          for (byte l = j + 8; l < j + 16; l++) {
            if (l > 23) {
              s_scheme[l - 24] = elem;
            } else {
              s_scheme[l] = elem;
            }
          }
            for (byte i = 0; i < 24; i++) {
              
              pixel.setPixelColor(i, s_scheme[i]);
              pixel.show();
            }
          if (j == 8) {
            refresh = false;
            break;
          }
          }
      } // Then normal setting display until press
      counter = map(sat, 0, 255, 0, 100);
      while (!shortpress) {
        /* 
          This function updates color scheme displayed 
        every time a pulse from the rotary encoder is 
        recorded. Press the rotary encoder button to save
        parameter.
        */
        click();
        sat = constrain(map(count(false, 0, 100), 0, 100, 0 , 250), 0, 255);
        colors();
        if (last_counter != counter || last_timer + update_ms <= timer) {
          last_counter = counter;
          last_timer = timer;
          for (byte i = 0; i < 24; i++) {
            switch (i)
            {
            case 0 ...7:
              pixel.setPixelColor(i, tri1);
              pixel.show();
              break;
            case 8 ...15:
              pixel.setPixelColor(i, tri2);
              pixel.show();
              break;
            case 16 ...23:
              pixel.setPixelColor(i, elem);
              pixel.show();
              break;
            }
          }
        }
      }
      refresh = true;
      break;
    
    }
  }
      
  void display() {
    
     n = constrain(map(time_running, 0, timeset, 0, 25), 0, 24);

      if (n != last_n || refresh) {
        last_n = n;
        refresh = false;
        for (byte j = 0; j < n; j++) {
          pixel.setPixelColor(j, elem);
          pixel.show();
        }
        for (byte i = n; i <= 23; i++ ) {
          pixel.setPixelColor(i, 0, 0, 0);
          pixel.show();
        }
      }
    
     
    
  }

  void clockStyle() {
    hour2 = time_running/3600;
    sec2 = time_running%3600;
    min2 = sec2/60;
    hour2 *= 2;
    min2 = map(min2, 0, 60, 0, 24);
    
    timeKeeper();  
    time_passed = minute*60 + second;
    time_running = timeset - time_passed;
    time_running = constrain(time_running, 0, timeset);
  // Sleep http://www.gammon.com.au/power
    if (time_running <= 0) {
      Going_To_Sleep();
    }

    // Update clock every second
    if (last_second != second) {
      last_second = second;
      s_index--;
      if (s_index <= 0) {
        s_index = 23;
      }
      for (byte i = 0; i < 24; i++) {
        
        if (i == s_index) {
          scheme[i] = tri2;
        }
        else if (i == min2) {
          scheme[i] = tri1;
        }
        else if (i == hour2 && i != 0) {
          scheme[i] = comp;
        }
        else if (i % 2 == 0) {
          scheme[i] = elem;
          }
        else {
          scheme[i] = 0;
        }
        pixel.setPixelColor(i, scheme[i]);
        pixel.show();
      }
    }
  }

  void trackPresses() {
    if (shortpress) {
      Serial.println("Short!");
    }
    else if(longpress) {
      Serial.println("Long!");
    }
    else if (doublepress) {
      Serial.println("Double!");
    }
  }

void transitionToDisplay() {
  timeKeeper();  
  time_passed = minute*60 + second;
  time_running = timeset - time_passed;
  time_running = constrain(time_running, 0, timeset);
  n = constrain(map(time_running, 0, timeset, 0, 25), 0, 24);

  for (byte i = 0; i < 24; i++) {
    last_timer = timer;
    while (timer < last_timer + 25) {}
    if (i > n) {
      pixel.setPixelColor(i, 0);
      pixel.show();
    } else {
      pixel.setPixelColor(i, elem);
      pixel.show();
      }
  }
  refresh = true;
  display();
  
}

