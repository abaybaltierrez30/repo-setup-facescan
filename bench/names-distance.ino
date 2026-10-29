// names-distance.ino: reads the HC-SR04, calls the person a name, answers each name its own way.

const int TRIG = 9;          // sensor TRIG to D9
const int ECHO = 10;         // sensor ECHO to D10

struct Row { const char* name; bool over; float line; float gap; float hold; };

// CHANGE: your State Table, one row per name, in cm. nobody comes last.
// Checked top to bottom: the first row that has stayed true for its whole hold wins.
Row rows[] = {
  // name        over LINE?  LINE   GAP   hold (s)
  { "present",   false,      100,    5,    1.0 },   // name 1: under 100 cm for 1 s
  { "nobody",    true,       100,    5,    5.0 },   // name 2: over 100 cm for 5 s
};
const int ROWS = sizeof(rows) / sizeof(rows[0]);

const int LED = 5;           // LED long leg to D5, short leg through a 220 ohm resistor to GND
const int BUZZ = 6;          // buzzer plus to D6, minus to GND

bool on[ROWS];               // is each row true right now?
unsigned long since[ROWS];    // when each row turned true
int current = ROWS - 1;      // the name now. It starts as nobody.

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);                     // a 10 microsecond ping
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long us = pulseIn(ECHO, HIGH, 30000);
  float cm = us ? us / 58.0 : 400.0;            // no echo back: call it far away
  unsigned long now = millis();

  for (int i = 0; i < ROWS; i++) {              // each row is the LINE and GAP from step 2
    bool was = on[i];
    float edge = rows[i].line;
    float gap = rows[i].gap;
    if (rows[i].over) {
      if (cm > edge + gap) on[i] = true;
      if (cm < edge - gap) on[i] = false;
    } else {
      if (cm < edge - gap) on[i] = true;
      if (cm > edge + gap) on[i] = false;
    }
    if (on[i] && !was) since[i] = now;          // it just turned true: start its clock
  }

  for (int i = 0; i < ROWS; i++) {              // top to bottom
    if (on[i] && now - since[i] >= rows[i].hold * 1000) {
      current = i;                              // the first row held long enough wins
      break;
    }
  }                                             // no row held yet? the name stays as it was

  switch (current) {                            // CHANGE: one response per name, no two the same
    case 0:                                     // intruder: light on, buzzer sounds
      digitalWrite(LED, HIGH);
      tone(BUZZ, 440, 100);
      break;
    case 1:                                     // hesitant: light blinks, no sound
      digitalWrite(LED, (now / 250) % 2);
      break;
    default:                                    // nobody: dark and quiet
      digitalWrite(LED, LOW);
  }

  Serial.print("cm ");                          // numbers only: bench.html skips words
  Serial.print(cm, 1);
  Serial.print(" name ");                       // name 0 is the top row
  Serial.println(current);

  delay(50);
}
