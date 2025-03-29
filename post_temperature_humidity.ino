#include <ArduinoJson.h>

// 定数定義
const unsigned long REQUEST_INTERVAL = 60000; // 60秒
const float TEMPERATURE = 70.1;
const float HUMIDITY = 60.2;

void printError(const String& message) {
  Serial.println("エラー: " + message);
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
  return jsonData;
}

void handleResponse(const String& response) {
  StaticJsonDocument<1024> responseDoc;
  DeserializationError error = deserializeJson(responseDoc, response);

  if (error) {
    printError("JSON解析エラー: " + String(error.c_str()));
    return;
  }

  // レスポンス解析（相手側のJSON構造に応じて変更が必要）
  float temperature = responseDoc["temperature"];
  float humidity = responseDoc["humidity"];

  Serial.println("\n受信データ:");
  Serial.print("temperature: ");
  Serial.println(temperature, 1);
  Serial.print("humidity: ");
  Serial.println(humidity, 1);
}

void sendSerialData() {
  String jsonData = createRequestJson();
  Serial.println("送信データ: " + jsonData);
  Serial.println(jsonData); // 実際にシリアル送信する行
}

void readSerialData() {
  static String receivedData;
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      if (receivedData.length() > 0) {
        Serial.println("受信生データ: " + receivedData);
        handleResponse(receivedData);
        receivedData = "";
      }
    } else {
      receivedData += c;
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(4000); // シリアル初期化待ち
  Serial.println("シリアル通信開始");
}

void loop() {
  static unsigned long lastSendTime = 0;
  unsigned long currentTime = millis();

  // 定期的にデータ送信
  if (currentTime - lastSendTime >= REQUEST_INTERVAL) {
    lastSendTime = currentTime;
    sendSerialData();
  }

  // 受信処理
  readSerialData();
}
