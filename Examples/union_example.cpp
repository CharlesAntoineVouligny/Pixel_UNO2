#include <Arduino.h>

//https://forum.arduino.cc/t/solved-splitting-and-rebuilding-of-long-data-type-numbers/606975/2
//..\Union Example\union_example.ino
//
typedef union u_conv
{
    long    lValue;
    byte    lByte[4];
        
};

u_conv
    strt,
    finish;
        
void setup() 
{
    Serial.begin(9600);
    
    strt.lValue = 2147483643;

    Serial.println("Original values:");
    Serial.println(strt.lValue, DEC);
    Serial.println(strt.lValue, BIN);

    //send start.lByte[0..3] to storage medium
    //
    //here, the retrieval is simulated by copying the byte values from the "strt" union
    //to the corresponding byet values in the "finish" union
    //
    //in use, you'd retrieve the bytes from storage and put them in a destination union
    for(int i = 0; i < 4; i++){
        finish.lByte[i] = strt.lByte[i];
    }
    Serial.println("Processed values:");
    Serial.println(finish.lValue, DEC);
    Serial.println(finish.lValue, BIN);

}//setup

void loop() 
{

}//loop