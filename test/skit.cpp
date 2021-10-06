int period = 1000;
unsigned long time_now = 0;
 
void setup() {
    Serial.begin(115200);
}
 
void loop() {
    time_now = millis();
   
    Serial.println("Hello");
   
    while(millis() < time_now + period){
        //wait approx. [period] ms
    }
}
