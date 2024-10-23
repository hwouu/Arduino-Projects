// Task 03-2

// LED에 사용할 핀 번호를 정의합니다.
const int ledPin = 16;  // LED가 연결된 GPIO 핀 번호

// 시리얼 입력 상태를 저장할 변수
char serState = NULL;

void setup() {
  // 115200의 속도로 시리얼 통신을 시작합니다.
  Serial.begin(115200);

  // LED 핀을 출력으로 설정합니다.
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 시리얼 포트에 데이터가 있는지 확인합니다.
  if (Serial.available() > 0) {
    // 시리얼 포트로부터 한 문자를 읽습니다.
    serState = Serial.read();
  }

  // 시리얼 모니터에서 'h'가 입력되었는지 확인합니다.
  if (serState == 'h') {
    // LED를 켭니다.
    digitalWrite(ledPin, HIGH);
  }
  // 시리얼 모니터에서 'l'이 입력되었는지 확인합니다.
  else if (serState == 'l') {
    // LED를 끕니다.
    digitalWrite(ledPin, LOW);
  }

  // 250ms 동안 지연시킵니다.
  delay(250);
}
