// Master
#include <SPI.h>

#define CS0 10
#define CS1 9

void setup(void) {
  Serial.begin(9600);
  pinMode(CS0, OUTPUT);
  pinMode(CS1, OUTPUT);
  
  // 초기 SS를 HIGH 상태로 설정
  digitalWrite(CS0, HIGH);
  digitalWrite(CS1, HIGH);
  
  SPI.begin();  // SPI 통신 시작
  SPI.setClockDivider(SPI_CLOCK_DIV8);  // 클럭 속도 설정
}

byte transferAndWait(const byte what) {
  SPDR = what;  // 데이터 전송
  while (!(SPSR & _BV(SPIF))) {  // 전송 완료 대기
  }
  return SPDR;  // 전송 받은 데이터 리턴
}

void loop(void) {
  if (Serial.available()) {
    char startChar = Serial.read();  // '#' 문자를 읽음
    if (startChar == '#') {
      while (!Serial.available()) {}  // 입력 대기
      char id = (char)Serial.read();  // Slave ID 읽음
      while (!Serial.available()) {}
      char blinkCount = (char)Serial.read();  // LED 깜빡임 횟수 읽음
      
      // Slave 선택
      switch (id) {
        case '0':
          digitalWrite(CS0, LOW);  // Slave0 선택
          break;
        case '1':
          digitalWrite(CS1, LOW);  // Slave1 선택
          break;
      }

      transferAndWait(blinkCount);  // blinkCount 전송
      byte ack = transferAndWait(0);  // ACK 수신

      Serial.println("Send '" + String(blinkCount - '0') + "' to Slave " + String(id - '0'));
      Serial.println("ACK: '" + String(ack - '0') + "'");

      digitalWrite(CS0, HIGH);  // Slave 선택 해제
      digitalWrite(CS1, HIGH);  // Slave 선택 해제
    }
  }
}
