#include <Arduino.h>

const int ledPin = 16; // the number of the LED pin
int ledState = LOW; // ledState used to set the LED
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = 1000; // interval at which to blink (milliseconds)
void setup() {
// set the digital pin as output:
pinMode(ledPin, OUTPUT);
}
void loop() {
// here is where you'd put code that needs to be running all the time.
unsigned long currentMillis = millis();
if (currentMillis - previousMillis >= interval) {
// save the last time you blinked the LED
previousMillis = currentMillis;
if (ledState == LOW) ledState = HIGH;
else ledState = LOW;
}
// set the LED with the ledState of the variable:
digitalWrite(ledPin, ledState);
}