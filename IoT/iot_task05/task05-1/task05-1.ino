const int trigPin = 22;
const int echoPin = 23;

void setup()
{
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop()
{
  long duration, distance;

  // Triggering by 10us pulse
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Getting duration for echo pulse
  duration = pulseIn(echoPin, HIGH);

  // Sound speed = 340 m/s = 34/1000 cm/us
  // Distance = duration * 34/1000 * 1/2
  distance = duration * 17 / 1000;

  Serial.print(distance);
  Serial.print("cm");
  Serial.println();

  delay(100);
}