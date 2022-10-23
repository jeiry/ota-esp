#include <ESP8266WiFi.h>
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("start");
}
long otaInterval = 0;
void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED)
  {
    if (millis() - otaInterval >= 60000) {
      //循环时间，不要少于1分钟(60000)，否则会失败
      otaInterval = millis();
      httpGetOTA();
    }
  } else {
    wifiInit(); //启动wifi
  }
}
int staStarted = 0;//0是未连接 1是连接中
void wifiInit() //初始化 WIFI
{
  if (staStarted == 0)
  {
    Serial.println("WIFI_STA");
    WiFi.mode(WIFI_STA);
    WiFi.begin("xxx", "xxx");
    staStarted = 1;
  }
  else if (staStarted == 1)
  {
    Serial.print(".");
    delay(1000);
  }
}
