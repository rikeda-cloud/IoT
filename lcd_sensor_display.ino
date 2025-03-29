#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Adafruit_AHTX0 aht;
#define SCL_PIN 18
#define SDA_PIN 21

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// 温度、湿度、2つの文字列を含む構造体
typedef struct {
    float temperature;   // 温度
    float humidity;      // 湿度
    char text1[50];      // 1つ目の文字列（最大50文字）
    char text2[50];      // 2つ目の文字列（最大50文字）
} SensorData;

void setup_lcd()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello,");
  lcd.setCursor(0, 1);
  lcd.print("world!");
}

void setup_censors()
{
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("I²C 初期化完了");
  if (!aht.begin()) {
    Serial.println("AHT21の初期化に失敗しました！");
    while (1); // 初期化失敗時は無限ループ
  }
  Serial.println("AHT21の初期化完了");
}

void setup() {
  Serial.begin(115200);
  delay(4000); // シリアルモニタの準備待ち
  setup_censors();
  setup_lcd();
}

void get_datas(SensorData *datas)
{
  sensors_event_t humidity, temp;
  
  aht.getEvent(&humidity, &temp); // AHT21から湿度と温度の取得

  // 温度と湿度を構造体に入れる。
  datas->temperature = temp.temperature;
  datas->humidity = humidity.relative_humidity;
}

void format_datas(SensorData *datas)
{
  sprintf(datas->text1, "temp:%dc", (int)round(datas->temperature));
  sprintf(datas->text2, " hmd:%d%%", (int)round(datas->humidity));
}

void print2lines(const char* text1, const char* text2)
{
    lcd.setCursor(0, 0); // 1行目に移動
    lcd.print(text1); // 1行目の文字列を表示
    lcd.setCursor(0, 1); // 2行目に移動
    lcd.print(text2); // 2行目の文字列を表示
    delay(2000);
    lcd.clear();  // LCDの内容をクリア
}

void displaySensorData(SensorData data) {
  print2lines(data.text1, data.text2);
}

void loop() {
  SensorData *datas = (SensorData *)malloc(sizeof(SensorData)); // メモリ確保
  if (datas == NULL) {
      Serial.println("メモリ確保失敗");
      return;
  };
  get_datas(datas);
  format_datas(datas);
  displaySensorData(*datas);
  free(datas);
  delay(1000); // 1秒待機
}
