click();
if (singlepress) {
  Serial.println("Single Press!");
  singlepress = false;
}
if (doublepress) {
  Serial.println("Double Press!");
  doublepress = false;
}
if (longpress) {
  Serial.println("Long Press!");
  longpress = false;
}
