#include <EEPROM.h>
#include <WiFi.h>
#include "time.h"

// Wi-Fi 설정
const char* ssid = "Galaxy Z Flip4";
const char* password = "1029384756";

WiFiServer server(8888);  // 웹 서버 포트

String header;
String output16State = "off";  // GPIO 16 상태 저장
String output17State = "off";  // GPIO 17 상태 저장

const int output16 = 16;  // LED 제어 핀
const int output17 = 17;
const int buzPin = 23;    // 부저 핀

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

int alarmHour = -1;  // 알람 시간 저장
int alarmMinute = -1;
bool alarmSet = false;

const char* ntpServer = "pool.ntp.org";  // NTP 서버
const long gmtOffset_sec = 3600 * 9;  // 한국 시간
const int daylightOffset_sec = 0;

// PWM 설정
const int ledChannel = 0;
const int resolution = 10;
const int duty = 128;

// EEPROM 관련 변수
const int maxSize = 100;
int noteCount = 0;
const int signature1 = 0xAA;
const int signature2 = 0x55;

// 음과 지속 시간
int nFrq[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 0};
int nDur[] = { 2000, 1500, 1000, 750, 500, 375, 250 };

// EEPROM에서 노트 및 지속시간 읽기
void playMusicFromEEPROM() {
  int savedNoteCount = EEPROM.read(2);
  for (int i = 0; i < savedNoteCount; i++) {
    int note = EEPROM.read(3 + i * 2);
    int dur = EEPROM.read(3 + i * 2 + 1);
    if (note != 13) {
      ledcSetup(ledChannel, nFrq[note], resolution);
      ledcWrite(ledChannel, duty);
    } else {
      ledcWrite(ledChannel, 0);
    }
    delay(nDur[dur]);
    ledcWrite(ledChannel, 0);
  }
}

String getLocalTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "시간을 가져올 수 없습니다.";
  }
  char timeStr[64];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeStr);
}

// 알람 체크 함수 추가
void checkAlarm() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    if (alarmSet && timeinfo.tm_hour == alarmHour && timeinfo.tm_min == alarmMinute) {
      Serial.println("알람 시간! EEPROM에 저장된 음악 재생.");
      playMusicFromEEPROM();  // 알람 시 EEPROM에 저장된 음악 재생
      alarmSet = false;  // 알람 해제
    }
  }
}

void handleClientRequest(WiFiClient& client) {
  if (header.indexOf("GET /16/on") >= 0) {
    Serial.println("GPIO 16 on");
    output16State = "on";
    digitalWrite(output16, HIGH);
  } else if (header.indexOf("GET /16/off") >= 0) {
    Serial.println("GPIO 16 off");
    output16State = "off";
    digitalWrite(output16, LOW);
  } else if (header.indexOf("GET /17/on") >= 0) {
    Serial.println("GPIO 17 on");
    output17State = "on";
    digitalWrite(output17, HIGH);
  } else if (header.indexOf("GET /17/off") >= 0) {
    Serial.println("GPIO 17 off");
    output17State = "off";
    digitalWrite(output17, LOW);
  } else if (header.indexOf("GET /setAlarm") >= 0) {  // 알람 설정
    String h = header.substring(header.indexOf("hour=") + 5, header.indexOf("&"));
    String m = header.substring(header.indexOf("minute=") + 7, header.indexOf(" HTTP/"));
    alarmHour = h.toInt();
    alarmMinute = m.toInt();
    alarmSet = true;
    Serial.println("알람이 설정되었습니다: " + String(alarmHour) + ":" + String(alarmMinute));
  }
}

void generateWebpage(WiFiClient& client) {
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta charset=\"UTF-8\">");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  client.println("<style>");
  client.println("html { font-family: Helvetica; text-align: center;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 30px; margin: 10px; cursor: pointer;}");
  client.println(".button2 { background-color: #555555; }");
  client.println(".inputField { font-size: 20px; padding: 10px; margin: 5px; }");
  client.println("form { margin: 20px 0; }");
  client.println("</style>");
  client.println("</head>");
  client.println("<body><h1>ESP32 Control Panel</h1>");

  // 실시간 시간 표시
  client.println("<p>현재 시간: <span id=\"time\">로딩 중...</span></p>");
  client.println("<script>");
  client.println("setInterval(function() {");
  client.println("var xhttp = new XMLHttpRequest();");
  client.println("xhttp.onreadystatechange = function() {");
  client.println("if (this.readyState == 4 && this.status == 200) {");
  client.println("document.getElementById(\"time\").innerHTML = this.responseText;");
  client.println("}");
  client.println("};");
  client.println("xhttp.open(\"GET\", \"/time\", true);");
  client.println("xhttp.send();");
  client.println("}, 1000);");  // 1초마다 시간 업데이트
  client.println("</script>");

  // GPIO 16 상태 표시 및 제어 버튼
  client.println("<p>GPIO 16 - State " + output16State + "</p>");
  if (output16State == "off") {
    client.println("<p><a href=\"/16/on\"><button class=\"button\">ON</button></a></p>");
  } else {
    client.println("<p><a href=\"/16/off\"><button class=\"button button2\">OFF</button></a></p>");
  }

  // GPIO 17 상태 표시 및 제어 버튼
  client.println("<p>GPIO 17 - State " + output17State + "</p>");
  if (output17State == "off") {
    client.println("<p><a href=\"/17/on\"><button class=\"button\">ON</button></a></p>");
  } else {
    client.println("<p><a href=\"/17/off\"><button class=\"button button2\">OFF</button></a></p>");
  }

  // 알람 설정 폼
  client.println("<h2>알람 설정</h2>");
  client.println("<form action=\"/setAlarm\" method=\"GET\">");
  client.println("시간 (24시간 형식): <input class=\"inputField\" type=\"number\" name=\"hour\" min=\"0\" max=\"23\" required>");
  client.println("분: <input class=\"inputField\" type=\"number\" name=\"minute\" min=\"0\" max=\"59\" required>");
  client.println("<br><input type=\"submit\" value=\"알람 설정\" class=\"button\"></form>");

  // 알람 상태 출력
  if (alarmSet) {
    client.println("<p>알람이 설정되었습니다: " + String(alarmHour) + ":" + String(alarmMinute) + "</p>");
  } else {
    client.println("<p>알람이 설정되지 않았습니다.</p>");
  }

  client.println("</body></html>");
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  
  pinMode(output16, OUTPUT);
  pinMode(output17, OUTPUT);
  ledcAttachPin(buzPin, ledChannel);
  
  digitalWrite(output16, LOW);
  digitalWrite(output17, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  // NTP 서버 설정
  
  // EEPROM에서 저장된 음악 유효성 확인
  if (EEPROM.read(0) == signature1 && EEPROM.read(1) == signature2) {
    Serial.println("EEPROM에 저장된 유효한 멜로디를 발견했습니다.");
  } else {
    Serial.println("EEPROM에 저장된 유효한 멜로디가 없습니다.");
  }
  
  server.begin();
}

void loop() {
  checkAlarm();
  
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            if (header.indexOf("GET /time") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println(getLocalTimeString());  // 현재 시간 전송
            } else {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              handleClientRequest(client);
              generateWebpage(client);
            }
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
    Serial.println("Client disconnected.");
  }
}