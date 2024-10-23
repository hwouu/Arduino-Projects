// Task07-A: LED 제어와 현재 시간을 표시하는 웹서버 구현
#include <WiFi.h>
#include <time.h>

// Wi-Fi 설정
const char* ssid = "Galaxy Z Flip4";  // 연결할 Wi-Fi SSID
const char* password = "1029384756";       // 연결할 Wi-Fi 비밀번호

// NTP 서버 정보 설정
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600 * 9;  // 한국 시간 (GMT+9)
const int daylightOffset_sec = 0;     // 일광 절약 시간 없음

// 웹서버 설정 (HTTP 기본 포트 80)
WiFiServer server(80);

// HTTP 요청을 저장할 문자열
String header;

// GPIO 상태 변수
String output16State = "off";
String output17State = "off";

// GPIO 핀 할당
const int output16 = 16;
const int output17 = 17;

// 시간 관리 변수
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;  // 타임아웃 시간 (2초)

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

// 웹 페이지를 렌더링하는 함수 (LED 상태 및 시간 표시)
void renderWebPage(WiFiClient& client) {
  // HTML 웹 페이지 생성
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

  // HTML 종료
  client.println("</body></html>");
  client.println();
}

void setup() {
  // 시리얼 통신 시작
  Serial.begin(115200);

  // GPIO 핀을 출력 모드로 설정
  pinMode(output16, OUTPUT);
  pinMode(output17, OUTPUT);

  // 기본적으로 GPIO 핀을 LOW로 설정 (LED 꺼짐)
  digitalWrite(output16, LOW);
  digitalWrite(output17, LOW);

  // Wi-Fi 연결 시작
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  // NTP 서버와 시간 동기화
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // 웹 서버 시작
  server.begin();
}

void loop() {
  // 클라이언트 접속 대기
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");  // 새로운 클라이언트 연결 메시지 출력
    String currentLine = "";  // 클라이언트 요청을 저장할 문자열
    currentTime = millis();
    previousTime = currentTime;

    // 클라이언트가 연결되어 있는 동안 처리
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {  // 클라이언트 요청이 있을 경우
        char c = client.read();  // 요청에서 문자를 읽고
        Serial.write(c);  // 시리얼 모니터에 출력
        header += c;

        // 두 개의 newline이 연속으로 들어오면 요청 종료
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // HTTP 응답 헤더 생성
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // 요청에 따라 GPIO 제어
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
            }

            // 웹 페이지 렌더링
            renderWebPage(client);

            // 요청 처리가 완료되었으므로 루프 종료
            break;
          } else {
            currentLine = "";  // 현재 라인 초기화
          }
        } else if (c != '\r') {  // 캐리지 리턴이 아닐 경우
          currentLine += c;  // 현재 라인에 문자를 추가
        }
      }
    }

    // 요청 헤더 초기화 및 클라이언트 연결 종료
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
