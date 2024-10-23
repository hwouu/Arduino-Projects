// Task 03-4

// 핀 번호 정의
const int touchPin = 4;    // 터치 핀이 연결된 GPIO 핀 번호 (T0)
const int ledPin = 16;     // LED가 연결된 GPIO 핀 번호

// 임계값 정의 (터치 센서 값이 이 값보다 작아지면 LED를 켜는 조건으로 설정)
const int threshold = 20;

// 터치값을 저장할 변수
int touchValue;

void setup() {
  // 115200의 속도로 시리얼 통신을 시작합니다.
  Serial.begin(115200);

  // 시리얼 모니터를 열 시간을 줍니다.
  delay(1000);

  // LED 핀을 출력으로 설정합니다.
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 터치 핀의 값을 읽어옵니다.
  touchValue = touchRead(touchPin);

  // 터치 값을 시리얼 모니터에 출력합니다.
  Serial.print(touchValue);

  // 터치 값이 임계값보다 작으면
  if (touchValue < threshold) {
    // LED를 켭니다.
    digitalWrite(ledPin, HIGH);
    Serial.println(" - LED on");
  }
  // 그렇지 않으면
  else {
    // LED를 끕니다.
    digitalWrite(ledPin, LOW);
    Serial.println(" - LED off");
  }

  // 500ms 동안 지연시킵니다.
  delay(500);
}
