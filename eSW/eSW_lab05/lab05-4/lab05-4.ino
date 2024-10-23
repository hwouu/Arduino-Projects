int pause = 0;           // Pause 상태를 저장 (0: 변화 중, 1: 정지 중)
int delta = 1;           // LED 밝기 변화 방향 (1: 밝아짐, -1: 어두워짐)
volatile byte state = LOW; // 스위치 상태 저장
int n = 0;               // PWM 값 (0 ~ 255)

// 스위치가 눌렸을 때 호출되는 인터럽트 서비스 루틴
void sw_pressed() {
  state = HIGH;          // 스위치가 눌리면 상태를 HIGH로 설정
  Serial.println("Interrupt: Button Pressed");
}

void setup() {
  pinMode(11, OUTPUT);   // PWM 핀 설정
  pinMode(2, INPUT);     // 스위치 핀 설정
  Serial.begin(9600);    // 시리얼 모니터 시작
  
  // 인터럽트 설정 (스위치가 눌렸을 때 sw_pressed 함수 호출)
  attachInterrupt(digitalPinToInterrupt(2), sw_pressed, RISING);
}

void loop() {
  if (pause == 0) {   // Pause가 아닌 경우에만 LED 밝기 변화
    analogWrite(11, n);   // 현재 PWM 값으로 LED 밝기 설정
    n += delta;           // 밝기를 증가 또는 감소
    
    // 밝기 값이 0 미만이거나 255를 넘으면 방향을 반대로 바꿈
    if (n >= 255 || n <= 0) {
      delta = -delta;
    }
    
    delay(10);            // LED 변화 속도를 제어
    Serial.println(n);     // 현재 PWM 값 출력
  }

  if (state == HIGH) {    // 스위치가 눌린 상태이면 Pause 상태 전환
    pause = pause ? 0 : 1;  // Pause를 토글 (0: 재생, 1: 정지)
    Serial.print("Pause 상태: ");
    Serial.println(pause);  // Pause 상태를 한 줄에 출력
    state = LOW;          // 스위치 상태 초기화
  }
}
