// Task 03-3

// 터치 핀 번호를 정의합니다.
const int touchPin = 4;  // 터치 핀이 연결된 GPIO 핀 번호 (T0)

// 설정 함수
void setup() {
  // 115200의 속도로 시리얼 통신을 시작합니다.
  Serial.begin(115200);

  // 시리얼 모니터를 시작할 시간을 줍니다.
  delay(1000);

  // 시리얼 모니터에 메시지를 출력합니다.
  Serial.println("ESP32 Touch Test");
}

void loop() {
  // 터치 핀의 값을 읽고 시리얼 모니터에 출력합니다.
  Serial.println(touchRead(touchPin));

  // 1초 동안 지연시킵니다.
  delay(1000);
}
