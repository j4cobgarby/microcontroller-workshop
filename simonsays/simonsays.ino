#include <Bounce2.h>

int btn_pins[] = {9,8,7,6};
int leds[] = {10,16,14,15};

Bounce btns[4];

unsigned int seq[64];
unsigned int seq_length;

unsigned int flash_delay = 600;

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
    btns[i].attach(btn_pins[i], INPUT_PULLUP);
    btns[i].interval(25);
  }

  randomSeed(analogRead(0));
  
  for (seq_length = 0; seq_length < 2; seq_length++) {
    seq[seq_length] = random(4);
  }
}

void show_seq() {
  for (int i = 0; i < seq_length; i++) {
    digitalWrite(leds[seq[i]], HIGH);
    delay(flash_delay);
    digitalWrite(leds[seq[i]], LOW);
    delay(200);
  }
}

int something_pressed() {
  for (int i = 0; i < 4; i++) {
    btns[i].update();
    if (btns[i].fell()) return i;
  }
  return -1;
}

// Return 1 if passed, 0 if made a mistake
int take_inputs() {
  int inp;
  
  for (int i = 0; i < seq_length; i++) {
    while ((inp=something_pressed()) == -1);
    digitalWrite(leds[inp], HIGH);
    delay(100);
    digitalWrite(leds[inp], LOW);
    if (seq[i] != inp) return 0;
  }

  return 1;
}

void loop() {
  delay(800);
  show_seq();
  int inp = take_inputs();
  Serial.println(inp);
  if (inp) {
    seq[seq_length++] = random(4);
    flash_delay -= 40;
    if (flash_delay < 200) flash_delay = 200;
  } else {
    Serial.println(":(");
    for (int i = 0; i < 5; i++) {
      for (int l = 0; l < 4; l++) {
        digitalWrite(leds[l], HIGH);
        delay(80);
        digitalWrite(leds[l], LOW);
        delay(80);
      }
    }

    for (seq_length = 0; seq_length < 2; seq_length++) {
      seq[seq_length] = random(4);
    }
    flash_delay = 600;
    delay(1000);
  }
}
