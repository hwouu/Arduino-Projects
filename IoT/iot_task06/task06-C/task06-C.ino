#include <EEPROM.h>

// PWM 설정
const int ledChannel = 0;
const int resolution = 8;
const int buzPin = 23;
const int duty = 128;
const int maxSize = 100;  // 저장할 수 있는 최대 노트-지속시간 쌍의 개수

// 변수
int vNote = 0, vDur = 0;
const int dDur = 250;  // 기본 지속시간
bool storingMusic = false;
int noteCount = 0;  // 저장된 노트의 개수
const int signature1 = 0xAA;
const int signature2 = 0x55;

// 음과 지속 시간
int nFrq[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 0};  // 노트 주파수 배열
int nDur[] = {2000, 1500, 1000, 750, 500, 375, 250};  // 노트 지속 시간 배열

// EEPROM을 지우는 함수
void clearEEPROM() {
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();  // 변경 사항을 저장
  Serial.println("EEPROM이 초기화되었습니다.");
}

// 음을 재생하는 함수
void playNote(int note, int dur) {
  if (note == 13) {
    // 쉼표 처리
    ledcWrite(ledChannel, 0);
  } else {
    // 주파수와 지속 시간을 설정
    ledcSetup(ledChannel, nFrq[note], resolution);
    ledcWrite(ledChannel, duty);
  }
  Serial.println(String(note) + "," + String(dur));  // 시리얼 모니터에 출력
  delay(nDur[dur]);  // 노트 지속 시간만큼 대기
  ledcWrite(ledChannel, 0);  // 노트 재생 후 소리 끔
}

// 노트 및 지속시간을 EEPROM에 저장
void storeNoteInEEPROM(int note, int dur) {
  if (noteCount < maxSize) {
    EEPROM.write(3 + noteCount * 2, note);  // EEPROM에 노트 저장
    EEPROM.write(3 + noteCount * 2 + 1, dur);  // EEPROM에 지속 시간 저장
    noteCount++;
    EEPROM.write(2, noteCount);  // EEPROM에 노트-지속시간 쌍 개수 저장
    EEPROM.commit();  // EEPROM에 저장 확정
  } else {
    Serial.println("EEPROM에 저장 공간이 부족합니다.");
  }
}

// EEPROM에서 노트 및 지속시간 읽어 음악 재생
void playMusicFromEEPROM() {
  int savedNoteCount = EEPROM.read(2);  // 저장된 노트-지속시간 쌍 개수
  for (int i = 0; i < savedNoteCount; i++) {
    int note = EEPROM.read(3 + i * 2);
    int dur = EEPROM.read(3 + i * 2 + 1);
    playNote(note, dur);  // 노트와 지속 시간으로 음악 재생
  }
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);  // EEPROM 초기화
  ledcAttachPin(buzPin, ledChannel);  // PWM 핀 설정

  // EEPROM에서 유효성 검사 후 음악 재생
  if (EEPROM.read(0) == signature1 && EEPROM.read(1) == signature2) {
    Serial.println("유효한 음악이 EEPROM에서 발견되었습니다. 음악 재생 중...");
    playMusicFromEEPROM();
  } else {
    Serial.println("EEPROM에 유효한 음악이 없습니다.");
  }
}

void loop() {
  // 시리얼 모니터에서 입력을 받아 음악 저장 및 재생 처리
  if (Serial.available() > 0) {
    char ch = Serial.read();  // 입력 읽기
    if (ch == '<') {
      storingMusic = true;
      noteCount = 0;  // 새로 저장 시작
      EEPROM.write(0, signature1);  // 유효성 서명 저장
      EEPROM.write(1, signature2);
      EEPROM.commit();
      Serial.println("음악 저장 시작");
    } else if (ch == '>') {
      storingMusic = false;
      Serial.println("음악 저장 완료");
    } else {
      vNote = ch;
      if (Serial.available() > 0) {
        vDur = Serial.read();  // 지속 시간 읽기
        if (vDur <= '6' && vDur >= '0') vDur -= '0';
        else vDur = dDur;

        // 노트 값을 변환
        if (vNote <= '9' && vNote >= '0')
          vNote -= '0';  // 숫자 노트 처리
        else if (vNote <= 'c' && vNote >= 'a')
          vNote = vNote - 'a' + 10;  // 알파벳 노트 처리 (쉼표는 'c'로 처리)
        else
          vNote = 13;  // 쉼표 처리

        // 노트 재생
        playNote(vNote, vDur);

        // 음악 저장 상태일 경우 EEPROM에 저장
        if (storingMusic) {
          storeNoteInEEPROM(vNote, vDur);
        }
      }
    }
  }
}
