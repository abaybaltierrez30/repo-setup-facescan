# System Charter: Distance

*Design target: this charter describes the intended experience; it does not claim that every behavior is implemented in the prototype yet.*

## What it senses

The ultrasonic sensor estimates how near or far a person is. It uses distance bands rather than presenting an exact measurement to the visitor.

| State | Distance band | Hold before changing state | Response |
|---|---:|---:|---|
| Close | 25 cm or less | 1 second | The light flashes rapidly and the buzzer beeps rapidly. |
| Medium | More than 25 cm and up to 50 cm | 1 second | The light blinks at a medium pace. |
| Far | More than 50 cm and up to 100 cm | 1 second | The light blinks slowly. |
| Gone | More than 100 cm | 1 second | The light is off; there is no beeping. |

The closer someone gets, the faster the light blinks. When no one is within 100 cm, the light stops.

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
