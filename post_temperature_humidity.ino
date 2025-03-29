#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// 定数定義
const char* SSID = "hi_guest";
const char* PASSWORD = "guestP@sS";
const char* OPENAI_API_URL = "https://postman-echo.com/post";
const unsigned long REQUEST_INTERVAL = 60000; // 60秒
const float TEMPERATURE = 70.1;
const float HUMIDITY = 60.2;

void printError(const String& message) {
  Serial.println("エラー: " + message);
}

bool connectToWiFi() {
  Serial.printf("Wi-Fiに接続中: %s\n", SSID);
  WiFi.begin(SSID, PASSWORD);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  Serial.println("\nWi-Fiに接続完了");
  Serial.print("IPアドレス: ");
  Serial.println(WiFi.localIP());
  return true;
}


String createRequestJson() {
  StaticJsonDocument<128> jsonDoc;
  jsonDoc["temperature"] = TEMPERATURE;
  jsonDoc["humidity"] = HUMIDITY;

  String jsonData;
  if (serializeJson(jsonDoc, jsonData) == 0) {
    Serial.println("Failed to serialize JSON");
    return "{}";
  }

  Serial.println("Generated JSON: " + jsonData);

  return jsonData;
}

void handleResponse(const String& response) {
  StaticJsonDocument<1024> responseDoc; // 元の512バイトでは不足する可能性あり
  DeserializationError error = deserializeJson(responseDoc, response);

  if (error) {
    printError("JSON解析エラー: " + String(error.c_str()));
    return;
  }

  // 正しいパスで値を取得（"json"オブジェクト内を参照）
  float temperature = responseDoc["json"]["temperature"];
  float humidity = responseDoc["json"]["humidity"];

  Serial.println("\n💬 temperature:");
  Serial.println(temperature, 1); // 小数点以下1桁表示
  Serial.println("\n💬 humidity:");
  Serial.println(humidity, 1);
}

void sendRequest() {
  if (WiFi.status() != WL_CONNECTED) {
    if (!connectToWiFi()) {
      printError("Wi-Fi再接続に失敗しました");
      return;
    }
  }

  HTTPClient http;
  http.begin(OPENAI_API_URL);
  http.addHeader("Content-Type", "application/json");

  String jsonData = createRequestJson();
  Serial.println("送信するデータ:");
  Serial.println(jsonData);

  int httpResponseCode = http.POST(jsonData);
  if (httpResponseCode <= 0) {
    printError("リクエスト失敗: " + String(http.errorToString(httpResponseCode)));
    http.end();
    return;
  }

  String response = http.getString();
  http.end();

  Serial.println("レスポンス:");
  Serial.println(response);
  handleResponse(response);
}

void setup() {
  Serial.begin(115200);
  delay(4000); // シリアルモニタの準備待ち

  if (!connectToWiFi()) {
    printError("Wi-Fi接続に失敗しました");
    return;
  }

  sendRequest();
}

void loop() {
  static unsigned long lastRequestTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastRequestTime >= REQUEST_INTERVAL) {
    lastRequestTime = currentTime;
    sendRequest();
  }
}

