# System Charter: The Doorman

The Doorman notices distance with one HC-SR04 ultrasonic sensor. The Arduino Nano runs its State Table and responses locally; no camera, laptop, or second sensor is needed for the piece to name a visitor.

| Name | Owner | Distance line | Hold | Response |
|---|---|---|---:|---|
| Near | Danny Girdish | Under 30 cm; enters below 25 cm and holds through 35 cm | 5 cm gap | 1 second | Steady red LED |
| Far | Alexander Bay Baltierrez | 30-100 cm; LINEs at 30 and 100 cm | 5 cm at each edge | 1 second | Slow red LED blink |
| Nobody | Group | Over 100 cm; enters above 105 cm and holds from 95 cm up | 5 cm gap | 5 seconds | LED off |

Each row must remain true for its hold before the name changes. Far enters above 35 cm and through 105 cm when coming from Near, or from 25 cm to below 95 cm when coming from Nobody; it stays active from 25-105 cm. The gap steadies readings at each line. A missing echo reads as 400 cm. The Nano prints the raw distance and current name for testing, but the visitor-facing response is the light and the printed state legend.

**For them:** Near holds the light on; Far slows it to a blink; Nobody leaves it dark.

**To them:** Danny's Near names closeness. Alexander's Far names distance. Nobody belongs to the group and treats absence as a reason to go quiet.

assignment/near-far-nobody
**It refuses:** to identify a face, remember a visitor, or count visits.
=======
## How it works

The ultrasonic sensor sends out a sound pulse and measures how long its echo takes to return. The prototype uses that time to estimate a distance in centimeters, then places the reading into one of the four bands above. A band must remain true for at least one second before the state changes, so brief fluctuations do not immediately change the response.

The light communicates the distance category through its blinking pace. The buzzer sounds rapidly for the Close state. The centimeter value is an internal sensor reading for development and processing; the visitor-facing response is categorical rather than an exact distance readout.

The laptop webcam is a separate part of the planned experience. It can provide face or expression signals, such as a smile, while the ultrasonic sensor provides the distance signal. The webcam is not what measures the centimeter distance. These signals may be combined to shape the experience.

## Experience

**For them:** The piece creates an experience that can suggest a surveillance tracker: it senses a visitor's presence and distance, alongside expression signals from the webcam.

**To them:** Visitors may feel watched because the installation responds to how close they are, as well as what the webcam senses.

**It refuses:** It does not present a visitor with an exact distance. The light communicates only a broad proximity category.

**It reminds me of:** *Very Nervous System* — close enough.

## First thumbnails

1. The prototype sits on a table. An ultrasonic sensor detects when someone approaches and estimates their distance. A webcam above it represents the expression-sensing part of the concept. As visitors approach, the distance response activates first; the webcam can then contribute expression signals.
2. The light communicates proximity through its blinking speed: faster means closer, slower means farther away, and no light means nobody is nearby. The Arduino and computer can use the centimeter reading internally to determine the Close, Medium, Far, or Gone state; visitors see the light response rather than an exact distance.

# Distance charter + webcam (soon)
It sees: distance and presence of a person.
It calls you: nobody, present, rebellious (future additinoal webcam)
It does: blinks on a red LED light (solid red LED light for now), turns off when presence not detected (blinks for now)
 For them:  Piece creates an experience that implies a surveillance, tracking, and detection.
 To them: Makes the person feel watched.
 It refuses: To provide exact distances, (for the webcam) to be actual in saving the person's info.
 Set aside:
 Sits next to: Webcam, reading someone's presence and distance, the webcam grabbing those interpretations and interpreting how the subject appears.
 The tension: Feeling of being tracked (even without save data), and the feeling of smile enforcement (for later in the piece).

--- Row ---
| present | 6.8, 10.0, 78.6 | 100 | 0 | 1 second | Visitor learns it by coming closer to the Arduino. | Piece emits a red light (progressively will flash faster and faster when updated) | For them: indicates their presence. / To them: makes them feel noticed by activating an outlet to let the person know they are. | Carlos B. |
|---|---|---|---|---|---|---|---|---|
| nobody | 400.0, 103.9, 129.1 cm | 100 | 0 | 0 s (could be fixed to 3 seconds) | Red LED flickers (could be updated to turn off instead) | D5: red LED on | For them: it indicates absence. To them: nothing. |  Visitor learns it by walking away or stepping back. | Carlos B. |
main
