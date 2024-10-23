#include <EEPROM.h>
#include <WiFi.h>
#include <time.h>

// Wi-Fi 설정
const char* ssid = "Galaxy Z Flip4";  // 연결할 Wi-Fi SSID
const char* password = "1029384756";       // 연결할 Wi-Fi 비밀번호

// NTP 서버 설정
const char* ntpServer = "pool.ntp.org";    // NTP 서버 주소
const long gmtOffset_sec = 3600 * 9;       // 한국 시간 (GMT+9)
const int daylightOffset_sec = 0;          // 일광 절약 시간 없음

// GPIO 설정
const int buzPin = 23;  // 부저 핀
const int output16 = 16;  // LED 제어용 GPIO 16
const int output17 = 17;  // LED 제어용 GPIO 17
String output16State = "off";
String output17State = "off";

// PWM 설정
const int ledChannel = 0;
const int resolution = 8;
const int duty = 64;  // 소리 크기를 줄이기 위한 duty 값

// EEPROM 설정
const int maxSize = 100;  // 최대 저장할 수 있는 노트 개수
const int signature1 = 0xAA;
const int signature2 = 0x55;

// 노트와 지속 시간 배열
int nFrq[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 0};  // 노트 주파수 배열
int nDur[] = {2000, 1500, 1000, 750, 500, 375, 250};  // 노트 지속 시간 배열

// 알람 시간
int alarmHour = -1;
int alarmMinute = -1;
bool alarmSet = false;

// 웹서버 설정
WiFiServer server(80);

// 현재 시간을 HTML 형식으로 반환하는 함수
String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Failed to obtain time";
  }
  char timeString[64];
  strftime(timeString, sizeof(timeString), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  return String(timeString);
}

// 현재 알람 시간이 되었는지 확인하는 함수
void checkAlarm() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    if (alarmSet && timeinfo.tm_hour == alarmHour && timeinfo.tm_min == alarmMinute) {
      Serial.println("Alarm time! Playing melody...");
      playMusicFromEEPROM();  // 알람 시간에 부저로 멜로디 재생
      alarmSet = false;  // 알람 해제
    }
  }
}

// 웹 페이지 렌더링 함수 (LED 상태 및 현재 시간 표시)
void renderWebPage(WiFiClient& client) {
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  client.println("<style>html { font-family: Helvetica; text-align: center;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #555555;}</style></head>");
  
  // 웹 페이지 제목
  client.println("<body><h1>ESP32 Web Server</h1>");
  
  // 현재 시간 표시
  client.println("<h2>Current Time: " + getFormattedTime() + "</h2>");

  // GPIO 16 상태 및 버튼 표시
  client.println("<p>GPIO 16 - State " + output16State + "</p>");
  if (output16State == "off") {
    client.println("<p><a href=\"/16/on\"><button class=\"button\">ON</button></a></p>");
  } else {
    client.println("<p><a href=\"/16/off\"><button class=\"button button2\">OFF</button></a></p>");
  }

  // GPIO 17 상태 및 버튼 표시
  client.println("<p>GPIO 17 - State " + output17State + "</p>");
  if (output17State == "off") {
    client.println("<p><a href=\"/17/on\"><button class=\"button\">ON</button></a></p>");
  } else {
    client.println("<p><a href=\"/17/off\"><button class=\"button button2\">OFF</button></a></p>");
  }

  // 알람 시간 설정 폼
  client.println("<h2>Set Alarm</h2>");
  client.println("<form action=\"/set-alarm\" method=\"GET\">");
  client.println("Hour: <input type=\"number\" name=\"hour\" min=\"0\" max=\"23\" required><br>");
  client.println("Minute: <input type=\"number\" name=\"minute\" min=\"0\" max=\"59\" required><br>");
  client.println("<input type=\"submit\" value=\"Set Alarm\">");
  client.println("</form>");

  if (alarmSet) {
    client.println("<p>Alarm is set for " + String(alarmHour) + ":" + String(alarmMinute) + "</p>");
  } else {
    client.println("<p>No alarm set</p>");
  }

  // HTML 종료
  client.println("</body></html>");
  client.println();
}

// 음을 재생하는 함수
void playNote(int note, int dur) {
  if (note == 13) {
    ledcWrite(ledChannel, 0);
  } else {
    ledcSetup(ledChannel, nFrq[note], resolution);
    ledcWrite(ledChannel, duty);
  }
  delay(nDur[dur]);
  ledcWrite(ledChannel, 0);  // 재생 후 소리 끔
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

  // Wi-Fi 연결 시작
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  
  // Wi-Fi 연결 대기
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // NTP 서버와 시간 동기화
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Time synchronized with NTP server.");

  // 웹 서버 시작
  server.begin();

  // 음악 재생을 위한 설정 (Wi-Fi 연결 후 실행)
  EEPROM.begin(512);  // EEPROM 초기화
  ledcSetup(ledChannel, 1000, resolution);  // PWM 초기화
  ledcAttachPin(buzPin, ledChannel);  // PWM 핀 설정

  // EEPROM에서 유효한 음악 데이터가 있는지 확인
  if (EEPROM.read(0) == signature1 && EEPROM.read(1) == signature2) {
    Serial.println("유효한 음악이 EEPROM에서 발견되었습니다. 음악 재생 중...");
    playMusicFromEEPROM();
  } else {
    Serial.println("EEPROM에 유효한 음악이 없습니다.");
  }
}

void loop() {
  WiFiClient client = server.available();  // 클라이언트 접속 대기
  if (client) {
    String header = "";  // HTTP 요청을 저장할 문자열
    String currentLine = "";  // 현재 라인 저장
    unsigned long currentTime = millis();
    unsigned long previousTime = currentTime;

    // 클라이언트가 연결되어 있는 동안 처리
    while (client.connected() && currentTime - previousTime <= 2000) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            // GPIO 16 제어
            if (header.indexOf("GET /16/on") >= 0) {
              output16State = "on";
              digitalWrite(output16, HIGH);
            } else if (header.indexOf("GET /16/off") >= 0) {
              output16State = "off";
              digitalWrite(output16, LOW);
            }

            // GPIO 17 제어
            if (header.indexOf("GET /17/on") >= 0) {
              output17State = "on";
              digitalWrite(output17, HIGH);
            } else if (header.indexOf("GET /17/off") >= 0) {
              output17State = "off";
              digitalWrite(output17, LOW);
            }

            // 알람 설정
            if (header.indexOf("GET /set-alarm") >= 0) {
              int hourIndex = header.indexOf("hour=") + 5;
              int minuteIndex = header.indexOf("minute=") + 7;
              alarmHour = header.substring(hourIndex, header.indexOf('&', hourIndex)).toInt();
              alarmMinute = header.substring(minuteIndex, header.indexOf(' ', minuteIndex)).toInt();
              alarmSet = true;
              Serial.println("Alarm set for " + String(alarmHour) + ":" + String(alarmMinute));
            }

            // 웹 페이지 렌더링
            renderWebPage(client);

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
  }

  checkAlarm();  // 알람 시간이 되었는지 확인
}
