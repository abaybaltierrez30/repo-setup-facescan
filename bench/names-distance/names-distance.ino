// names-distance.ino: the standalone Nano sketch for The Doorman.

const int TRIG = 9;          // sensor TRIG to D9
const int ECHO = 10;         // sensor ECHO to D10
const int LED = 5;           // LED long leg to D5, short leg through a 220 ohm resistor to GND

enum Band { BELOW, BETWEEN, ABOVE };

struct Row {
  const char* name;
  Band band;
  float lower;
  float upper;
  float gap;
  unsigned long holdMs;
};

// LINE and GAP are in cm. Holds are independent and start when each row wins.
Row rows[] = {
  // name      band      lower  upper  GAP  hold
  { "near",    BELOW,       0,     30,   5, 1000UL },
  { "far",     BETWEEN,    30,    100,   5, 1000UL },
  { "nobody",  ABOVE,     100,      0,   5, 5000UL },
};
const int ROWS = sizeof(rows) / sizeof(rows[0]);

int current = -1;
int candidate = -1;
unsigned long candidateSince = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);
}

bool rowIsTrue(const Row& row, float cm, bool active) {
  if (!active) {
    if (current < 0) {
      if (row.band == BELOW) return cm < row.upper;
      if (row.band == BETWEEN) return cm >= row.lower && cm <= row.upper;
      return cm > row.lower;
    }

    if (row.band == BELOW) return cm < row.upper - row.gap;
    if (row.band == BETWEEN) {
      if (current == 0) {
        return cm > row.lower + row.gap && cm <= row.upper + row.gap;
      }
      if (current == ROWS - 1) {
        return cm >= row.lower - row.gap && cm < row.upper - row.gap;
      }
      return cm > row.lower + row.gap && cm < row.upper - row.gap;
    }
    return cm > row.lower + row.gap;
  }

  if (row.band == BELOW) return cm <= row.upper + row.gap;
  if (row.band == BETWEEN) {
    return cm >= row.lower - row.gap && cm <= row.upper + row.gap;
  }
  return cm >= row.lower - row.gap;
}

int winningRow(float cm) {
  for (int i = 0; i < ROWS; i++) {
    if (rowIsTrue(rows[i], cm, current == i)) return i;
  }
  return -1;
}

void updateName(float cm, unsigned long now) {
  const int winner = winningRow(cm);
  if (winner < 0 || winner == current) {
    candidate = -1;
    return;
  }

  if (winner != candidate) {
    candidate = winner;
    candidateSince = now;
  }

  if (now - candidateSince >= rows[candidate].holdMs) {
    current = candidate;
    candidate = -1;
  }
}

void answer(unsigned long now) {
  if (current == 0) {
    digitalWrite(LED, HIGH);
  } else if (current == 1) {
    digitalWrite(LED, (now / 500UL) % 2 == 0 ? HIGH : LOW);
  } else {
    digitalWrite(LED, LOW);
  }
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

  updateName(cm, now);
  answer(now);

  Serial.print("cm ");
  Serial.print(cm, 1);
  Serial.print(" name ");
  Serial.println(current < 0 ? "starting" : rows[current].name);

  delay(50);
}
