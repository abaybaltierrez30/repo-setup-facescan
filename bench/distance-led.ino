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
