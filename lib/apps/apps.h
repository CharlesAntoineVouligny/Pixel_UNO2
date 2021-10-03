void timeKeeper();
void button();
void modeSelect();

void update();
void click();

void wakeUp();
void Going_To_Sleep();

long compHue();
void triadicHue();
/// If counter is above max, goes back to 0
/// If it's under 0, goes back to max
int roundCounter(int);

void clockArray();
void colors();
/// Make quarters exclusive
int exclusive(byte, byte min, byte max);
///Set the time
void timeSet();
void display();
/// Updates colors as user adjusts parameters
void settingDisplay();
void chase();
void blink();
void inverted_chase();
void clockStyle();
void trackPresses();
/// When going out of settings and back to default mode
void transitionToDisplay();