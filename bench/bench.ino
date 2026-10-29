// bench.ino: the Arduino half of a bench pair's sensor.
//
// It reads the sensor and prints one line per reading over USB, as name and
// number pairs separated by spaces:
//
//   cm 42.5
//   cm 41.9 light 612
//
// That is all it does. The meaning (the signals, the State Table, the piece) is
// built in the browser by bench/sense-serial.js, the same as the camera pairs.
//
// Wired for an HC-SR04 ultrasonic sensor on an Arduino Nano:
//   VCC -> 5V    GND -> GND    TRIG -> D9    ECHO -> D10
// To add a second sensor, read it in loop() and print it on the same line,
// before the println, e.g. a photoresistor divider on A0:
//   Serial.print(" light "); Serial.print(analogRead(A0));

const int TRIG = 9;
const int ECHO = 10;

void setup() {
  Serial.begin(115200);          // sense-serial.js opens the port at this speed
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  // The echo time in microseconds; 0 means nothing came back within 30 ms.
  unsigned long us = pulseIn(ECHO, HIGH, 30000);
  float cm = us ? us / 58.0 : 400.0; // nothing in range reads as far away

  Serial.print("cm ");
  Serial.print(cm, 1);
  Serial.println();

  delay(50);                     // about 20 readings a second
}
// distance-led.ino: reads the HC-SR04, prints cm, lights an LED when something is close.

const int TRIG = 9;          // sensor TRIG to D9
const int ECHO = 10;         // sensor ECHO to D10
const int LED = 5;           // LED long leg to D5, short leg through a 220 ohm resistor to GND
const int LINE = 30;         // CHANGE: centimetres where it should react
const int GAP = 5;           // raise this if the LED flickers at the line

bool near = false;           // remembers the last answer between readings

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);                    // a 10 microsecond ping
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long us = pulseIn(ECHO, HIGH, 30000);
  float cm = us ? us / 58.0 : 400.0;           // no echo back: call it far away

  if (cm < LINE - GAP) near = true;            // clearly closer than the line
  if (cm > LINE + GAP) near = false;           // clearly back past it
  digitalWrite(LED, near ? HIGH : LOW);       // the response

  Serial.print("cm ");                         // same format bench.html reads
  Serial.println(cm, 1);

  delay(50);
}