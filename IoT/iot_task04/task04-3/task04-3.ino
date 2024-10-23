// Task04-1: LED 밝기 제어

#include "driver/ledc.h"

// LED 핀 번호 정의
const int ledPin = 16;   // GPIO16에 LED 연결

// PWM 설정
const int freq = 5000;   // PWM 주파수: 5000Hz
const int ledChannel = 0;  // PWM 채널: 0번 사용
const int resolution = 8;  // PWM 해상도: 8비트 (0~255 값 사용)

void setup() {
  // LED에 PWM 기능을 설정
  ledcSetup(ledChannel, freq, resolution);

  // 해당 채널을 GPIO 핀에 연결
  ledcAttachPin(ledPin, ledChannel);
}

void loop() {
  // LED 밝기 증가
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
    // PWM을 사용하여 LED 밝기 변경
    ledcWrite(ledChannel, dutyCycle);
    delay(15);  // 지연 시간을 통해 변화 속도 조절
  }

  // LED 밝기 감소
  for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
    // PWM을 사용하여 LED 밝기 변경
    ledcWrite(ledChannel, dutyCycle);
    delay(15);  // 지연 시간을 통해 변화 속도 조절
  }
}
