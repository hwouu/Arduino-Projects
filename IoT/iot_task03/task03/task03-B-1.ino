// Task 03-1

// 푸시버튼과 LED에 사용할 핀 번호를 정의합니다.
const int buttonPin = 15;  // 푸시버튼이 연결된 GPIO 핀 번호
const int ledPin = 16;     // LED가 연결된 GPIO 핀 번호

// 푸시버튼 상태를 저장할 변수
int buttonState = 0;

void setup() {
  // 115200의 속도로 시리얼 통신을 시작합니다.
  Serial.begin(115200);

  // 푸시버튼 핀을 입력으로 설정합니다.
  pinMode(buttonPin, INPUT);

  // LED 핀을 출력으로 설정합니다.
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 푸시버튼의 현재 상태를 읽어옵니다.
  buttonState = digitalRead(buttonPin);
  Serial.println(buttonState);

  // 푸시버튼이 눌렸는지 확인합니다 (buttonState가 HIGH인 경우)
  if (buttonState == HIGH) {
    // LED를 켭니다.
    digitalWrite(ledPin, HIGH);
  } else {
    // LED를 끕니다.
    digitalWrite(ledPin, LOW);
  }
}
