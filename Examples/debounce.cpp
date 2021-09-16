// https://forum.arduino.cc/t/long-short-press-buttons-using-one-shared-interrupt-pin-solved/215074

const int button[] = {
  10,9,8,7};
//stores if the switch was high before at all
volatile int state[] = {
  LOW, LOW, LOW, LOW};
//storing the button state for short press mode
volatile int state_short[] = {
  LOW, LOW, LOW, LOW};
//storing the button state for long press mode
volatile int state_long[] = {
  LOW, LOW, LOW, LOW};
//stores the time each button went high or low
volatile unsigned long current_high[4];
volatile unsigned long current_low[4];


void setup()
{
  for(int i=0; i<4; i++)
  {
    pinMode(button[i], INPUT);
  }
  attachInterrupt(0, read_button, CHANGE);
}


void loop()
{
  for(int i=0; i<4; i++)
  {
    if(state[0] == HIGH)
    {
      //do something when button 0 was pressed short ...
      state[0] = LOW;
    }    
        if(state_long[3] == HIGH)
    {
      //do something when button 3 was pressed long ...
      state[0] = LOW;
    } 
  }
}


//is called when the Interrupt Pin is pressed or released (CHANGE Mode)
void read_button()
{
  //cycles through the buttons to find out which one was pressed or released
  for(int i=0; i<4; i++)
  {
    //if this is true the button was just pressed down
    if(digitalRead(button[i]) == HIGH)
    {
      //note the time the button was pressed
      current_high[i] = millis();
      state[i] = HIGH;
    }
    //if no button is high one had to be released. The millis function will increase while a button is hold down the loop function will be cycled (no change, so no interrupt is active) 
     if(digitalRead(button[i] == LOW) && state[i] == HIGH)
    {
      current_low[i] = millis();
      if((current_low[i] - current_high[i]) > 50 && (current_low[i] - current_high[i]) < 800)
      {
        state_short[i] = !state_short[i];
        state[i] = LOW;
      }
      else if((current_low[i] - current_high[i]) >= 800 && (current_low[i] - current_high[i]) < 4000)
      {
        state_long[i] = !state_long[i];
        state[i] = LOW;
      }
    }
  }
}

