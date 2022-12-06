#include <Bounce2.h>

int btn_pins[] = {9,8,7,6}; // GPIO pin to attach each button to
int leds[] = {10,16,14,15}; // GPIO pin to attach each LED to

Bounce btns[4]; // Bounce objects handle debouncing of buttons.

unsigned int seq[64]; // Sequence of button presses for the user to remember.
unsigned int seq_length;

unsigned int flash_delay = 600; // Time between sequence flashes decreases as the game progresses

void setup() {
  Serial.begin(9600); // Set up serial connection to host computer
  
  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT); // Configure all the LEDs as output pins
    btns[i].attach(btn_pins[i], INPUT_PULLUP); // Set up the Bounce objects with the correct pins
    btns[i].interval(25); // Milliseconds delay for debouncing
  }

  randomSeed(analogRead(0)); // Reading voltage on an unconnected analogue pin will give us a "random" value, that we use to seed the RNG.
  
  for (seq_length = 0; seq_length < 2; seq_length++) { // Initialise the sequence to a length of 2
    seq[seq_length] = random(4);
  }
}

// This function will display the current sequence using the LEDs.
// Just a simple loop to iterate each number in the sequence, then
// lighting the corresponding LED.
void show_seq() {
  for (int i = 0; i < seq_length; i++) {
    digitalWrite(leds[seq[i]], HIGH);
    delay(flash_delay);
    digitalWrite(leds[seq[i]], LOW);
    delay(200);
  }
}

// Use the Bounce objects to check if a button
// has been pressed (gone from High to Low voltage
// since last time we checked).
// Returns the button which was pressed.
int something_pressed() {
  for (int i = 0; i < 4; i++) {
    btns[i].update();
    if (btns[i].fell()) return i;
  }
  return -1;
}

// Wait for the user to input the sequence.
// If they make a mistake along the way, this will return 0.
// If they succeed, returns 1.
int take_inputs() {
  int inp;
  
  for (int i = 0; i < seq_length; i++) {
    while ((inp=something_pressed()) == -1); // "Busy" waiting for a button input
    
    digitalWrite(leds[inp], HIGH);
    delay(100);
    digitalWrite(leds[inp], LOW);
    if (seq[i] != inp) return 0;
  }

  return 1;
}

// Main loop of the program. This function is called constantly forever.
void loop() {
  delay(800);
  show_seq();
  int inp = take_inputs();
  Serial.println(inp);
  if (inp) { // If the sequence was inputted correctly by the user...
    seq[seq_length++] = random(4); // ...then just add a new number to the sequence.
    flash_delay -= 40; // Here we're speeding the game up until a certain threshold.
    if (flash_delay < 200) flash_delay = 200;
  } else { // If the user made a mistake
    Serial.println(":(");
    for (int i = 0; i < 5; i++) { // These nested loops flash the LEDs angrily.
      for (int l = 0; l < 4; l++) {
        digitalWrite(leds[l], HIGH);
        delay(80);
        digitalWrite(leds[l], LOW);
        delay(80);
      }
    }

    for (seq_length = 0; seq_length < 2; seq_length++) { // Reset the sequence
      seq[seq_length] = random(4);
    }
    flash_delay = 600; // Reset the game speed
    delay(1000);
  }
}
