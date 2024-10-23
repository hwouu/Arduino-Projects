#include <SPI.h>

#define CS0 10  // Chip Select for Slave0 (덧셈)
#define CS1 9   // Chip Select for Slave1 (곱셈)

void setup() {
  Serial.begin(9600);
  SPI.begin();
  
  pinMode(CS0, OUTPUT);
  pinMode(CS1, OUTPUT);
  
  digitalWrite(CS0, HIGH);  // Slave0 비활성화
  digitalWrite(CS1, HIGH);  // Slave1 비활성화
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');  // 시리얼 모니터로부터 입력 받음
    char operation = command.charAt(0);  // 명령어 'a' 또는 'm' (덧셈 또는 곱셈)
    
    // 덧셈 명령어 처리 (Slave0에 전달)
    if (operation == 'a') {
      String numbers = command.substring(1, command.length() - 1);  // 숫자 추출
      char terminator = command.charAt(command.length() - 1);       // 종료 문자 확인
      
      if (terminator == '.') {
        digitalWrite(CS0, LOW);  // Slave0 선택 (CS0 LOW)
        Serial.println("Master: Addition..");
        
        int sum = 0;  // 덧셈을 위한 초기화
        Serial.print(sum);  // 초기값 출력
        Serial.print(" ");
        
        for (unsigned int i = 0; i < numbers.length(); i++) {
          char digit = numbers.charAt(i);
          SPI.transfer(digit);  // Slave0으로 숫자 전송
          delay(10);  // Slave가 처리할 시간 제공
          
          sum += (digit - '0');  // 덧셈 계산
          Serial.print(sum);  // 중간 결과 출력
          Serial.print(" ");
        }
        
        SPI.transfer('.');  // 덧셈 종료 명령 전송
        delay(10);
        
        byte result = SPI.transfer(0);  // 최종 결과 수신
        digitalWrite(CS0, HIGH);  // Slave0 비활성화
        
        Serial.println();
        Serial.print("Result (Sum): ");
        Serial.println(result);  // 결과 출력
      }
    }

    // 곱셈 명령어 처리 (Slave1에 전달)
    else if (operation == 'm') {
      String numbers = command.substring(1, command.length() - 1);  // 숫자 추출
      char terminator = command.charAt(command.length() - 1);       // 종료 문자 확인
      
      if (terminator == '.') {
        digitalWrite(CS1, LOW);  // Slave1 선택 (CS1 LOW)
        Serial.println("Master: Multiplication..");
        
        int product = 1;  // 곱셈을 위한 초기값
        Serial.print(product);  // 초기값 출력
        Serial.print(" ");
        
        for (unsigned int i = 0; i < numbers.length(); i++) {
          char digit = numbers.charAt(i);
          SPI.transfer(digit);  // Slave1으로 숫자 전송
          delay(10);  // Slave가 처리할 시간 제공
          
          product *= (digit - '0');  // 곱셈 계산
          Serial.print(product);  // 중간 결과 출력
          Serial.print(" ");
        }
        
        SPI.transfer('.');  // 곱셈 종료 명령 전송
        delay(10);
        
        byte result = SPI.transfer(0);  // 최종 결과 수신
        digitalWrite(CS1, HIGH);  // Slave1 비활성화
        
        Serial.println();
        Serial.print("Result (Product): ");
        Serial.println(result);  // 결과 출력
      }
    }
  }
}
