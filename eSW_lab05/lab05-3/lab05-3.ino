void setup() {
  pinMode(11, OUTPUT);  // PWM 핀을 출력 모드로 설정
  Serial.begin(9600);   // 시리얼 모니터 시작
}

void loop() {
  // LED 밝기 증가 (0에서 255까지)
  for (int i = 0; i <= 255; i++) {
    analogWrite(11, i);  // LED 밝기를 i로 설정 (PWM 값 0~255)
    delay(10);           // 딜레이를 줘서 부드럽게 변화하도록 함
    Serial.println(i);   // 현재 밝기 값 출력
  }

  // LED 밝기 감소 (255에서 0까지)
  for (int i = 255; i >= 0; i--) {
    analogWrite(11, i);  // LED 밝기를 i로 설정 (PWM 값 255~0)
    delay(10);           // 딜레이를 줘서 부드럽게 변화하도록 함
    Serial.println(i);   // 현재 밝기 값 출력
  }
}
