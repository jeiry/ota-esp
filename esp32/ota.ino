#include <HTTPClient.h>
#include <ESP32httpUpdate.h>

#include <ArduinoJson.h>

#define VER 0.2
#define OTA_URL "http://noteapi.yoyolife.fun"
#define OTA_GID "88888888"

DynamicJsonDocument docota(1024); //内存池


void httpGetOTA() {
  String unionid = String((uint32_t)ESP.getEfuseMac(), HEX);
  unionid.toUpperCase();
  String url = "/api/ota/ver?v=" + String(VER) + "&id=" + unionid + "&gid=" + String(OTA_GID);
  String jsonStr = "";
  HTTPClient http;

  String serverPath = OTA_URL + url;

  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    jsonStr = http.getString();
    Serial.println(jsonStr);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  auto error = deserializeJson(docota, jsonStr); //解析消息
  if (error) { //检查解析中的错误
    Serial.println("Json 解析错误" + String(error.c_str()));
    return;
  }
  if (bool(docota["status"]) == true) {
    Serial.print("最新：");
    Serial.println(atof(docota["data"]["ver"]));
    Serial.print(" 当前：");
    Serial.println(String(VER).toFloat());
    if (atof(docota["data"]["ver"]) > String(VER).toFloat()) {
      Serial.println("开始升级");
      otaUpdate(docota["data"]["url"]);
    } else {
      Serial.println("不需要升级");
    }
  } else {
    String r = docota["msg"];
    Serial.println(r);
  }

}
void otaUpdate(String API_URL) {
  Serial.println(API_URL);
  ESPhttpUpdate.rebootOnUpdate(true);
  t_httpUpdate_return ret = ESPhttpUpdate.update(API_URL);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.println("[update] Update failed.");
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("[update] Update no Update.");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("[update] Update ok.");
      break;
  }
}
