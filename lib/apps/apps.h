void timeKeeper();
void button();
void modeSelect();

void update();
/// Counts pulses
/// set first param to true for a round counter
/// counts from min to max then back to min
/// if round is false then it is a straight counter
/// from min to max
byte count(bool round, byte min, byte max);
void click();

void wakeUp();
void Going_To_Sleep();

void colors();
void clockArray();
/// Make quarters exclusive
int exclusive(byte, byte min, byte max);
///Set the time
void timeSet();
/// Updates colors as user adjusts parameters
void settingDisplay();
void display();

void clockStyle();
void trackPresses();
/// When going out of settings and back to default mode
void transitionToDisplay();

