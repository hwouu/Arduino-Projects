// Task03 - Step C (수정된 코드)

// 핀 번호 정의
const int buttonPin = 15;  // Tact 스위치가 연결된 GPIO 핀 번호
bool pauseState = false;   // Pause 상태를 저장하는 변수
bool buttonPressed = false; // 스위치가 눌렸는지 여부를 저장하는 변수
bool lastPauseState = false; // Pause 상태가 마지막으로 출력되었는지 저장하는 변수

void setup() {
  // 115200의 속도로 시리얼 통신을 시작합니다.
  Serial.begin(115200);
  
  // 스위치 핀을 입력으로 설정합니다.
  pinMode(buttonPin, INPUT);
}

void loop() {
  // 스위치 상태를 읽습니다.
  int buttonState = digitalRead(buttonPin);

  // 스위치가 눌렸는지 확인
  if (buttonState == HIGH && !buttonPressed) {
    // Pause 상태를 토글합니다.
    pauseState = !pauseState;
    buttonPressed = true;  // 스위치가 눌렸음을 기록
  }
  
  // 스위치가 다시 눌리지 않았을 때 상태를 초기화합니다.
  if (buttonState == LOW) {
    buttonPressed = false;
  }

  // Pause 상태일 때만 한 번 메시지를 출력
  if (pauseState) {
    if (!lastPauseState) {
      Serial.println("Paused");
      lastPauseState = true; // Pause 메시지가 한 번만 출력되도록 설정
    }
  } else {
    // Pause 상태가 아니면 "Data is streaming..."을 지속적으로 출력
    Serial.println("Data is streaming...");
    lastPauseState = false;  // 다시 Pause 메시지가 출력될 수 있도록 초기화
  }

  // 500ms 지연 (너무 빠른 토글을 방지)
  delay(500);
}
