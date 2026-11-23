// oled_and_led_distancedemo.ino: Near/Far/Nobody distance states with OLED and LED output.
//
// Same serial output as bench.ino ("cm 42.5" lines at 115200), so
// sense-serial.js works unchanged. The screen is extra. On the two-colour
// panels the top 16 rows are yellow: that strip is the status (state badge,
// readings per second). The blue below is the data: the distance, a sonar ping
// whose blip sits at the measured range, and a scrolling trace of the history.
//
// Wiring, Arduino Nano:
//   HC-SR04  VCC -> 5V    GND -> GND    TRIG -> D9    ECHO -> D10
//   OLED     VCC -> 5V    GND -> GND    SDA  -> A4    SCL  -> A5
//   LED      long leg -> D5; short leg -> 220 ohm resistor -> GND
// The OLED is an SSD1306, 128 x 64, I2C at 0x3C. Install "U8g2" by olikraus
// from Arduino IDE's Library Manager before compiling this sketch.

#include <U8g2lib.h>
#include <Wire.h>

// Page mode: draws the screen in eight strips with a 128-byte buffer, because
// the Nano has 2 KB of RAM and a full frame would take half of it.
U8G2_SSD1306_128X64_NONAME_1_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

const int TRIG = 9;
const int ECHO = 10;
const int LED = 5;

const float MAX_CM = 200.0;      // top of the graph and the sonar range

enum Band { BELOW, BETWEEN, ABOVE };

struct Row {
  const char* name;
  Band band;
  float lower;
  float upper;
  float gap;
  unsigned long holdMs;
};

Row rows[] = {
  { "near",    BELOW,       0,     30,   5, 1000UL },
  { "far",     BETWEEN,    30,    100,   5, 1000UL },
  { "nobody",  ABOVE,     100,      0,   5, 5000UL },
};
const int ROWS = sizeof(rows) / sizeof(rows[0]);

const int GRAPH_H = 10;          // the trace, rows 53 to 63
uint8_t hist[128];               // one column per reading, 0..GRAPH_H
uint8_t head = 0;

float smooth = MAX_CM;
float sweep = 0;                 // sonar sweep angle, degrees
int sweepDir = 1;
uint8_t ping = 0;                // expanding ring radius
uint8_t rate = 0;                // readings in the last second, for the header
uint8_t count = 0;
unsigned long rateStart = 0;
int current = -1;
int candidate = -1;
unsigned long candidateSince = 0;

float readCm() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long us = pulseIn(ECHO, HIGH, 30000);
  return us ? us / 58.0 : 400.0;
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

void splash() {
  for (uint8_t r = 2; r < 70; r += 6) {
    oled.firstPage();
    do {
      oled.drawCircle(64, 32, r);
      if (r > 8) oled.drawCircle(64, 32, r - 8);
      oled.setFont(u8g2_font_6x12_tr);
      oled.drawStr(28, 36, "SENSOR LAYER");
    } while (oled.nextPage());
  }
}

void drawHeader(int state) {
  // Rows 0-15, the yellow strip: state badge left, readings per second right.
  const char *label = state < 0 ? "STARTING" : rows[state].name;
  oled.setFont(u8g2_font_6x12_tr);
  int w = oled.getStrWidth(label) + 8;
  if (state == 0) {
    oled.drawRBox(0, 1, w, 13, 3);
    oled.setDrawColor(0);
    oled.drawStr(4, 11, "NEAR");
    oled.setDrawColor(1);
  } else {
    oled.drawRFrame(0, 1, w, 13, 3);
    oled.drawStr(4, 11, state < 0 ? label : state == 1 ? "FAR" : "NOBODY");
  }
  char buf[8];
  oled.setFont(u8g2_font_5x8_tr);
  itoa(rate, buf, 10);
  strcat(buf, "/s");
  int rw = oled.getStrWidth(buf);
  oled.drawStr(127 - rw, 10, buf);
  // a pulse that blinks once per reading, beside the rate
  if (head & 1) oled.drawDisc(127 - rw - 6, 7, 2);
  else oled.drawCircle(127 - rw - 6, 7, 2);
}

void drawNumber(float cm) {
  char buf[8];
  oled.setFont(u8g2_font_logisoso22_tn);
  // The smoothed value creeps toward 400 (no echo) but never reaches it.
  if (cm >= 399.0) {
    strcpy(buf, "---");
  } else {
    dtostrf(cm, 0, cm < 100 ? 1 : 0, buf);
  }
  oled.drawStr(0, 45, buf);
  int x = oled.getStrWidth(buf) + 2;
  oled.setFont(u8g2_font_5x8_tr);
  oled.drawStr(x, 45, "cm");
}

void drawSonar(float cm) {
  const int ox = 99, oy = 50, R = 28;
  // range rings and the frame of the fan
  oled.drawCircle(ox, oy, R / 3, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  oled.drawCircle(ox, oy, 2 * R / 3, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  oled.drawCircle(ox, oy, R, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  // the outgoing ping
  if (ping < R) oled.drawCircle(ox, oy, ping, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  // the sweep line
  float a = (sweep - 90) * DEG_TO_RAD;
  oled.drawLine(ox, oy, ox + R * cos(a), oy + R * sin(a));
  // the blip, at the measured range along the sweep
  if (cm < MAX_CM) {
    float r = cm / MAX_CM * R;
    int bx = ox + r * cos(a), by = oy + r * sin(a);
    oled.drawDisc(bx, by, cm < rows[0].upper ? 3 : 2);
  }
}

void drawGraph() {
  // A trace, oldest at the left: a line reads cleaner than solid bars, on the
  // panel and in a photo of it.
  const int y0 = 63;
  for (uint8_t x = 1; x < 128; x++) {
    uint8_t a = hist[(head + x - 1) % 128];
    uint8_t b = hist[(head + x) % 128];
    oled.drawLine(x - 1, y0 - a, x, y0 - b);
  }
}

void setup() {
  Serial.begin(115200);          // sense-serial.js opens the port at this speed
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);
  oled.begin();
  oled.setBusClock(400000);
  splash();
}

void loop() {
  float cm = readCm();
  unsigned long now = millis();
  updateName(cm, now);
  answer(now);

  Serial.print("cm ");
  Serial.print(cm, 1);
  Serial.println();

  smooth += (min(cm, 400.0f) - smooth) * 0.35;

  count++;
  if (millis() - rateStart >= 1000) {
    rate = count;
    count = 0;
    rateStart = millis();
  }

  // closer reads taller in the graph
  float c = constrain(smooth, 0, MAX_CM);
  hist[head] = (uint8_t)((1.0 - c / MAX_CM) * GRAPH_H);
  head = (head + 1) % 128;

  sweep += 6 * sweepDir;
  if (sweep > 60 || sweep < -60) sweepDir = -sweepDir;
  ping = (ping + 3) % 36;

  oled.firstPage();
  do {
    drawHeader(current);
    drawNumber(smooth);
    drawSonar(smooth);
    drawGraph();
  } while (oled.nextPage());
}
