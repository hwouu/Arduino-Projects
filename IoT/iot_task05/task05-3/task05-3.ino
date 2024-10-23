#define timeSeconds 3  // LED가 켜져 있을 시간 (3초)

const int led = 16;           // LED 핀
const int motionSensor = 36;  // PIR 센서 핀

unsigned long now = millis();  // 현재 시간
unsigned long lastTrigger = 0; // 마지막 모션 감지 시간
boolean startTimer = false;    // 타이머 상태 변수

// 모션 감지 시 인터럽트 핸들러
void IRAM_ATTR detectsMovement() {
    Serial.println("모션 감지됨!");
    digitalWrite(led, HIGH);  // LED 켜기
    startTimer = true;        // 타이머 시작
    lastTrigger = millis();   // 모션 감지 시간 저장
}

void setup() {
    Serial.begin(115200);

    pinMode(motionSensor, INPUT_PULLUP);  // PIR 센서를 INPUT_PULLUP 모드로 설정
    pinMode(led, OUTPUT);  // LED 핀을 출력으로 설정
    digitalWrite(led, LOW);  // 초기 상태로 LED 끄기

    // 모션 감지 시 인터럽트 설정 (RISING 모드)
    attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
}

void loop() {
    now = millis();

    // 3초가 지나면 LED 끄기
    if (startTimer && (now - lastTrigger > (timeSeconds * 1000))) {
        Serial.println("모션 종료...");
        digitalWrite(led, LOW);  // LED 끄기
        startTimer = false;      // 타이머 리셋
    }
}