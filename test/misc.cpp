#include <Arduino.h>
#include <EEPROM.h>

byte bright, sat;
long hue;

union u_conv
{
    long    lValue;
    byte    lByte[4];
        
};

u_conv
    strt,
    finish;

void setup() {
    Serial.begin(9600);

    bright = EEPROM.read(1);
    Serial.println(bright);
    delay(10);
    sat = EEPROM.read(2);
    Serial.println(sat);
    delay(10);

    for(int i = 3; i < 7; i++){
            finish.lByte[i-3] = EEPROM.read(i);
        }
    hue = finish.lValue;
    delay(10);

    Serial.println(hue);
    delay(10);
}
void loop(){
        delay(500);
    }
