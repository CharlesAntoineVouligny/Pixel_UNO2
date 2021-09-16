/*
This program turns on and off a LED on pin 13 each 1 second using an internal timer
*/
#include <Arduino.h>
int timer=0;
bool state=0;
void setup() {
  DDRD = 0b10000000;
    
  TCCR0A=(1<<WGM01);    //Set the CTC mode   
  OCR0A=0xF9; //Value for ORC0A for 1ms 
  
  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
  sei(); //Enable interrupt
  
  TCCR0B|=(1<<CS01);    //Set the prescale 1/64 clock
  TCCR0B|=(1<<CS00);

 
    
}

void loop() {
  //in this way you can count 1 second because the nterrupt request is each 1ms
  if(timer>=1000){
    state=!state;
    timer=0;
  }
  
  PORTD ^= (state << 7);
  
}

ISR(TIMER0_COMPA_vect){    //This is the interrupt request
  timer++;
}