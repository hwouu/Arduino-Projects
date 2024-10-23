#include "driver/ledc.h"

// 초음파 센서 핀 번호
const int trigPin = 22;
const int echoPin = 23;

// 피에조 부저 핀 및 PWM 설정
const int piezoPin = 21;       // 피에조 부저가 연결된 GPIO 핀
const int piezoChannel = 0;    // PWM 채널
const int resolution = 8;      // 8비트 해상도 (0~255)

// 거리 및 주파수 매핑 설정
const int minDistance = 2;    // 최소 측정 거리 (2cm)
const int maxDistance = 100;  // 최대 측정 거리 (100cm)
const int minFreq = 500;      // 최소 주파수 (500Hz)
const int maxFreq = 5000;     // 최대 주파수 (5000Hz)

void setup() {
  // 시리얼 통신 시작
  Serial.begin(115200);
  
  // 초음파 센서 핀 설정
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // 피에조 부저에 PWM 설정
  ledcSetup(piezoChannel, minFreq, resolution);
  ledcAttachPin(piezoPin, piezoChannel);
}

void loop() {
  long duration, distance;

  // 초음파 신호 발사
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Echo 핀에서 반사 시간 측정
  duration = pulseIn(echoPin, HIGH);

  // 거리를 cm 단위로 계산
  distance = duration * 17 / 1000;

  // 거리 값을 주파수 범위로 매핑 (물체가 가까울수록 높은 주파수 발생)
  int toneFreq = map(distance, minDistance, maxDistance, maxFreq, minFreq);
  
  // 피에조 부저에서 해당 주파수로 소리 발생
  ledcWriteTone(piezoChannel, toneFreq);

  // 시리얼 모니터에 거리 및 주파수 출력
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Tone Frequency: ");
  Serial.println(toneFreq);
  
  delay(100);  // 100ms마다 거리 및 주파수 갱신
}
