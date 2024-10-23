// Task03 - Step A

// 핀 번호 정의
const int buttonPin = 15;  // Tact 스위치가 연결된 GPIO 핀 번호
const int touchPin = 4;    // 터치 핀이 연결된 GPIO 핀 번호 (T0)
const int ledPin = 16;     // LED가 연결된 GPIO 핀 번호

// 임계값 설정
const int threshold = 20;  // 터치 센서 임계값

// 변수 정의
int buttonState = 0;       // 스위치 상태 저장 변수
int touchValue;            // 터치 값 저장 변수

void setup() {
  // 115200의 속도로 시리얼 통신을 시작합니다.
  Serial.begin(115200);

  // 핀 모드 설정
  pinMode(buttonPin, INPUT);    // 스위치 핀을 입력으로 설정
  pinMode(ledPin, OUTPUT);      // LED 핀을 출력으로 설정

  // 시리얼 모니터 시작 시간을 줍니다.
  delay(1000);
}

void loop() {
  // 스위치 상태 읽기
  buttonState = digitalRead(buttonPin);

  // 터치 값 읽기
  touchValue = touchRead(touchPin);

  // 스위치 상태와 터치 값을 시리얼 모니터에 출력
  Serial.print("Button: ");
  Serial.print(buttonState);
  Serial.print(" | Touch: ");
  Serial.println(touchValue);

  // 스위치가 눌리거나 터치 값이 임계값보다 작으면
  if (buttonState == HIGH || touchValue < threshold) {
    // LED를 켭니다.
    digitalWrite(ledPin, HIGH);
    Serial.println("LED on");
  }
  // 그렇지 않으면
  else {
    // LED를 끕니다.
    digitalWrite(ledPin, LOW);
    Serial.println("LED off");
  }

  // 500ms 지연
  delay(500);
}
