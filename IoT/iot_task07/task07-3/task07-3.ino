#include <WiFi.h>
#include <time.h>

// Wi-Fi 설정
const char* ssid = "Galaxy Z Flip4";  // 연결할 Wi-Fi SSID
const char* password = "1029384756";       // 연결할 Wi-Fi 비밀번호

// NTP 서버 정보 설정
const char* ntpServer = "pool.ntp.org";    // NTP 서버 주소
const long gmtOffset_sec = 3600 * 9;       // 한국 시간 (GMT+9)
const int daylightOffset_sec = 0;          // 일광 절약 시간 (없음)

// 현재 시간을 출력하는 함수
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");  // 요일, 월, 일, 년, 시간 출력
  Serial.println("Year: " + String(timeinfo.tm_year + 1900) +
                 ", Month: " + String(timeinfo.tm_mon + 1) +
                 ", Day: " + String(timeinfo.tm_mday) +
                 ", Hour: " + String(timeinfo.tm_hour) +
                 ", Minute: " + String(timeinfo.tm_min) +
                 ", Second: " + String(timeinfo.tm_sec));
}

void setup() {
  // 시리얼 통신 시작
  Serial.begin(115200);

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
  Serial.println("Time synchronized with NTP server.");

  // 초기 시간 출력
  printLocalTime();

  // Wi-Fi 연결 해제 (더 이상 필요 없으므로 전력 절약)
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop() {
  // 1초마다 현재 시간 출력
  delay(1000);
  printLocalTime();
}
