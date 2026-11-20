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

**It refuses:** to identify a face, remember a visitor, or count visits.
