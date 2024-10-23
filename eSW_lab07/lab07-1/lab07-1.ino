void setup() {
  pinMode(12, OUTPUT);  // Pin 12에 연결된 LED 설정
  Serial.begin(9600);   // 시리얼 통신 시작 (보드레이트 9600bps)
}

void loop() {
  if (Serial.available()) {  // 시리얼 데이터가 있을 때
    if (Serial.read() == '#') {  // 데이터의 시작이 '#'인지 확인
      while (!Serial.available());
      char id = (char)Serial.read();  // Client ID 읽기

      while (!Serial.available());
      char blinkCount = (char)Serial.read();  // LED 깜빡임 횟수 읽기

      // Client ID와 깜빡임 횟수가 유효한지 확인
      if (id == '0' && (blinkCount >= '0' && blinkCount <= '9')) {
        int blinks = blinkCount - '0';  // 문자를 숫자로 변환

        // LED를 지정된 횟수만큼 깜빡이기
        blinkLED(12, blinks);
      }
    }
  }
}

// LED를 지정된 횟수만큼 깜빡이는 함수
void blinkLED(int pin, int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(pin, HIGH);  // LED 켜기
    delay(500);               // 500ms 대기
    digitalWrite(pin, LOW);   // LED 끄기
    delay(500);               // 500ms 대기
  }
}
