#include <EEPROM.h>

// PWM 설정
const int ledChannel = 0;
const int resolution = 8;
const int buzPin = 23; // 부저가 연결된 핀
const int duty = 128;  // PWM의 듀티 사이클 (음량)

// 변수 설정
int vNote = 0, vDur = 0;
const int dDur = 250;  // 기본 지속 시간

// 노트 주파수 배열
int nFreq[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523};

// 노트 지속 시간 배열
int nDur[] = {2000, 1500, 1000, 750, 500, 375, 250};

// PWM으로 노트를 재생하는 함수
void playNote(int note, int dur) {
  if (note == -1) {
    // 쉼표일 때 (음이 없을 때)
    ledcSetup(ledChannel, 0, resolution);
    ledcWrite(ledChannel, 0);
  } else {
    // 주파수와 지속 시간 설정
    ledcSetup(ledChannel, nFreq[note], resolution);
    ledcWrite(ledChannel, duty);
  }
  Serial.println(String(note) + "," + String(dur));  // 시리얼 모니터에 출력
  delay(nDur[dur]);  // 노트 지속 시간만큼 대기
}

void setup() {
  Serial.begin(115200);
  ledcAttachPin(buzPin, ledChannel);

  // EEPROM에서 데이터를 읽어와 음악을 재생
  if (EEPROM.read(0) == 0xAA && EEPROM.read(1) == 0x55) {
    int numPairs = EEPROM.read(2);  // 저장된 노트-지속 시간 쌍의 개수
    for (int i = 0; i < numPairs; i++) {
      int note = EEPROM.read(3 + 2 * i);
      int dur = EEPROM.read(4 + 2 * i);
      playNote(note, dur);  // 저장된 데이터를 사용해 노트 재생
    }
  } else {
    Serial.println("유효한 데이터가 EEPROM에 없습니다.");
  }
}

void loop() {
  // 시리얼 모니터에서 노트와 지속 시간을 입력 받기
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == '<') {
      // 음악 저장 시작
      int index = 3;
      while (Serial.available() > 0 && Serial.peek() != '>') {
        vNote = Serial.read();  // 노트 읽기
        vDur = Serial.read();   // 지속 시간 읽기
        EEPROM.write(index++, vNote);  // EEPROM에 노트 저장
        EEPROM.write(index++, vDur);   // EEPROM에 지속 시간 저장
      }
      EEPROM.write(0, 0xAA);  // 유효성 서명
      EEPROM.write(1, 0x55);
      EEPROM.write(2, (index - 3) / 2);  // 노트-지속 시간 쌍의 수 저장
      EEPROM.commit();  // EEPROM에 저장 확정
      Serial.println("음악이 EEPROM에 저장되었습니다.");
    } else if (input == 'p') {
      // EEPROM에서 음악 재생
      int numPairs = EEPROM.read(2);  // 노트-지속 시간 쌍의 개수
      for (int i = 0; i < numPairs; i++) {
        int note = EEPROM.read(3 + 2 * i);
        int dur = EEPROM.read(4 + 2 * i);
        playNote(note, dur);  // 노트와 지속 시간으로 음악 재생
      }
    }
  }
}
