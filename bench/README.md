# The bench track

The physical prototype uses one HC-SR04 and an Arduino Nano. The Nano runs the Near, Far, and Nobody rows and their LED responses on its own; `bench.html` is only an optional browser readout. Do not add a second sensor until all names work with this one.

**Simple sensor first.** Every pair builds with sensors from the shelf below, the simple tier, and gets them working in the browser before anything else. An advanced sensor is a second sensor, added after the simple one works, never instead of it.

## Building it

The kits come apart, so building the circuit is part of your work.

Important: in the Arduino IDE, each sketch must live in its own folder. Open the folder that matches the sketch you want to upload, such as `bench/bench/` for the simple distance sensor, then click **Upload**. Do not try to upload `src/main.cpp` from this repo in the Arduino IDE; that file is only for PlatformIO.

1. Use a Nano, breadboard, HC-SR04, LED, 220 ohm resistor, and jumpers. If the Nano's header pins are not soldered, ask Mr. Reasy before soldering.
2. Wire one sensor and test its numbers in the Arduino IDE Serial Monitor at 115200. Close the monitor before any browser connection.
3. `bench/names-distance/names-distance.ino` is the stand-alone three-name build. `bench/bench/bench.ino` is the optional raw serial reader for `bench.html`.

## The reference build

This one works, end to end: a Nano and an HC-SR04 on a breadboard, with an optional 0.96 in screen, read by `bench.html` in Chrome. Build yours to match first, then change it.

![The reference build: an Arduino Nano, an ultrasonic distance sensor and a 0.96 in OLED screen on a breadboard. The screen reads NEAR, 23.6 cm, with a sonar fan and a history trace.](img/build-overview.jpg)

| Part | Pin | Nano pin |
|---|---|---|
| HC-SR04 | VCC | 5V |
| HC-SR04 | GND | GND |
| HC-SR04 | TRIG | D9 |
| HC-SR04 | ECHO | D10 |
| OLED (optional) | VCC | 5V |
| OLED (optional) | GND | GND |
| OLED (optional) | SDA | A4 |
| OLED (optional) | SCL | A5 |

![The Nano close up, USB end at the top. Two wires on the right side at D9 and D10 go to the sensor; on the left side, wires at A4 and A5 go to the screen, and 5V and GND feed both.](img/nano-pins.jpg)

*The Nano's pins: D9 and D10 on the right go to the sensor; A4, A5, 5V and GND on the left.*

![A US-025A ultrasonic sensor, front view, pins labelled VCC, Trig, Echo, GND along the bottom edge.](img/sensor-pins.jpg)

*Ours is a US-025A, a clone of the HC-SR04. Same four pins in the same order (VCC, Trig, Echo, GND), same wiring, same sketch. Either board works.*

Flash `bench/names-distance/names-distance.ino` from its folder in the Arduino IDE. The Nano starts in a waiting state and begins naming only after a row has held. Near enters below 25 cm; Far enters above 35 cm and through 105 cm from Near, or from 25 cm to below 95 cm from Nobody; Nobody enters above 105 cm. Their holds are 1, 1, and 5 seconds. To try the optional browser mirror instead, upload `bench/bench/bench.ino`, open `bench.html` on localhost in Chrome, connect the Arduino, and press **D**.

### Standalone Arduino examples

The `distance-led` folder is an earlier one-state LED example. `names-distance` is the finished stand-alone state-table sketch. Open each folder's `.ino` file as its own sketch in the Arduino IDE; Arduino compiles `.ino` files together when they share a sketch folder.

**The lag is the table, not the wire.** Near and Far each hold for 1 second; Nobody holds for 5 seconds. In the Nano sketch, `GAP` is 5 cm at each line. If the LED flickers near a line, increase `GAP`; if the response feels too slow, adjust that row's hold.

**If the upload fails** with `not in sync` or `programmer is not responding`, the Nano has the old bootloader, which is common on clone boards. In the Arduino IDE choose **Tools → Processor → ATmega328P (Old Bootloader)** and upload again (in `arduino-cli`, the board is `arduino:avr:nano:cpu=atmega328old`). After a failed upload, unplug the Nano and plug it back in before the next try, because the port can stay stuck.

### The screen (optional)

`bench/oled_demo/oled_demo.ino` is `bench.ino` plus a readout on a 0.96 in SSD1306 screen (128 x 64, I2C): a state badge, the distance in big digits, a sonar sweep with a dot at the measured range, and a scrolling trace of the history. It prints the same lines over USB, so `bench.html` works with it unchanged. It needs the **U8g2** library (Arduino IDE → Library Manager). On the two-colour screens the top strip is yellow and holds the status.

![Three photos of the screen side by side: EMPTY reading 400 cm with no echo, HERE at 51.5 cm, NEAR at 7.2 cm with a large dot on the sonar.](img/screen-states.jpg)

*Three states, three thresholds: EMPTY when nothing echoes back (this photo predates the fix that shows `---` instead of 400), HERE closer than 120 cm, NEAR closer than 30 cm. That's a State Table, drawn on a screen. Yours will have different words and different numbers.* The screen's badge thresholds are for the screen only; your real State Table still lives in the browser. It's an extra for after the sensor works, never instead of it.

## What's on the shelf

| Sensor | A signal it can give | Watch for |
|---|---|---|
| HC-SR04 ultrasonic | `near`, `present` | The worked example in `bench.ino`. It reports the nearest thing in a cone about 30° wide, so flat on a desk it hears the desk: aim it up or off the edge. Soft clothing swallows the echo. |
| PIR motion | `moving` | On or off, and it holds itself on for a few seconds. Someone standing still disappears. |
| Photoresistor | `shade` | Needs a 10k resistor beside it. The room's light changes through the day. |
| TCRT5000 reflective IR | `close` | A hand within a few centimetres, nothing further. |
| Sound sensor (KY-037/038) | `loud` | Read its analog pin (A0), which gives a level. The digital pin only says over or under its dial. |
| TTP223 touch pad | `touch` | Works through a few millimetres of paper or plastic. |
| Piezo disc | `knock` | It spikes for an instant on a tap, so give its state a hold. |
| Tilt switch | `tilted` | For a piece someone picks up. |

Every sensor here prints a number; the meaning is yours. A `near` of 0.9 is a distance, never a feeling.

## Advanced sensing

Mr. Reasy's own bench sensors, lent to pairs whose simple sensor already works in the browser. Ask him for one; they live on his bench, not the shelf. Several need a 3.3 V board with more memory than the Nano: the Nano 33 IoT (north racks, a few) or an ESP32-S3 from his bench.

| Sensor | A signal it can give | How it talks | Board | Difficulty |
|---|---|---|---|---|
| MPR121 capacitive touch (12 pads) | `touched`, and which pad | I2C | Nano | Moderate. Any conductive thing becomes a pad: foil, fruit, graphite. |
| RC522 RFID reader | `tagged`, and which card | SPI | Nano, 3.3 V power only | Moderate. Each card has its own ID, so a piece can know which object is on it. |
| AS7341 / AS7343 spectral sensor | `colour`, `bright` | I2C | Nano | Moderate. Eight to fourteen colour bands; it needs steady light on the object. |
| BNO080/085 IMU | `tilted`, `turning`, `still` | I2C | Nano 33 IoT or ESP32-S3 | Hard. Gives orientation directly; too big for the Nano's memory. |
| INMP441 microphone | `loud` | I2S | Nano 33 IoT or ESP32-S3 | Hard. A real digital microphone; the Nano has no I2S. |
| HLK-LD2450 radar | `near`, `present`, `count`, where people stand | UART at 256000 baud | Nano 33 IoT | Hard. Tracks up to three people by position, through thin walls and in the dark. |
| Load cell, 10 kg or 15 kg bar | `loaded`, `weight` | Needs an amplifier board (HX711 or NAU7802) | Nano | Moderate once the amplifier is in hand. Ask Mr. Reasy whether one is free before you plan on it. |

Same rule as the shelf: the sensor gives a number and your State Table gives it a meaning. A radar `count` of 2 is two bodies in range, not a crowd that wants anything.
