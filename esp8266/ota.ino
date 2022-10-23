WiFiClient httpclient;
#include <ArduinoJson.h>
DynamicJsonDocument doc(1024); //内存池

#define VER 0.2
#define OTA_URL "http://noteapi.yoyolife.fun"
#define OTA_GID "88888888"

void httpGetOTA() {
  uint64_t chipid = ESP.getChipId();
  String unionid = String((uint32_t)chipid);
  
  /**
     测试是否正常连接
  */
  if (!httpclient.connect(OTA_URL, 80)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/api/ota/ver?v=" + String(VER) + "&id=" + unionid + "&gid=" + String(OTA_GID);
  String postRequest = (String)("GET ") + url + " HTTP/1.1\r\n" +
                       "Content-Type: text/html;charset=utf-8\r\n" +
                       "Host: " + OTA_URL + "\r\n" +
                       "User-Agent: BuildFailureDetectorESP8266\r\n" +
                       "Connection: Keep Alive\r\n\r\n";
  //  Serial.println(postRequest);
  httpclient.print(postRequest);  // 发送HTTP请求

  /**
     展示返回的所有信息
  */
  String line = httpclient.readStringUntil('\n');
  String jsonStr = "";
  while (line.length() != 0) {
    if (line.indexOf("{\"status\"") != -1) {
      jsonStr = String(line);
      break;
    }
    line = httpclient.readStringUntil('\n');
  }
  Serial.println(jsonStr);
  httpclient.stop();

  auto error = deserializeJson(doc, jsonStr); //解析消息
  if (error) { //检查解析中的错误
    Serial.println("Json 解析错误" + String(error.c_str()));
    return;
  }
  if (bool(doc["status"]) == true) {
    Serial.print("最新：");
    Serial.print(String(doc["data"]["ver"]).toFloat());
    Serial.print(" 当前：");
    Serial.println(String(VER).toFloat());
    if (String(doc["data"]["ver"]).toFloat() > String(VER).toFloat()) {
      Serial.println("开始升级");
      String result ;
      result = ota(String(doc["data"]["url"]));
      if (result == "updateok") {
        Serial.println("重启");
        ESP.restart();
      }
    } else {
      Serial.println("不需要升级");
    }
  }

}
//升级
#include <ESP8266httpUpdate.h>
String ota(String API_URL) {
  ESPhttpUpdate.rebootOnUpdate(true);
  t_httpUpdate_return ret = ESPhttpUpdate.update(httpclient, API_URL);
  String result ;
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      result = "failed";
      Serial.println("[update] Update failed.");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      result = "noupdate";
      Serial.println("[update] Update no Update.");
      break;
    case HTTP_UPDATE_OK:
      result = "updateok";
      Serial.println("[update] Update ok."); // may not be called since we reboot the ESP
      break;
  }
  Serial.println("--- ota");

  return result;


}
