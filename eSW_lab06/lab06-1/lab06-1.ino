const int ledPin = 13;  // Pin for LED 13
int blinkCount = 0;     // Variable to store the blink count

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);  // Initialize hardware serial communication
}

void loop() {
  if (Serial.available() > 0) {
    blinkCount = Serial.parseInt();  // Read the blink count from the client
    blinkLED(blinkCount);            // Call the blinkLED function
  }
}

void blinkLED(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(ledPin, HIGH);  // Turn the LED on
    delay(500);                  // Wait for 500 ms
    digitalWrite(ledPin, LOW);   // Turn the LED off
    delay(500);                  // Wait for 500 ms
  }
}