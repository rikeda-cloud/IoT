#include <driver/gpio.h>

const int SW_PIN1 = 17;
const int SW_PIN2 = 18;

uint8_t data = 0; // 8ビットのデータを格納する変数
int bitCount = 0; // 現在のビット数

void setup() {
  pinMode(SW_PIN1, INPUT);
  gpio_set_pull_mode(GPIO_NUM_17, GPIO_PULLUP_ONLY);

  pinMode(SW_PIN2, INPUT);
  gpio_set_pull_mode(GPIO_NUM_18, GPIO_PULLUP_ONLY);

  Serial.begin(115200);
}

void loop() {
  int value1 = digitalRead(SW_PIN1) == LOW ? 1 : -1; // SW_PIN1 が押されたら 1、押されてなければ -1
  int value2 = digitalRead(SW_PIN2) == LOW ? 0 : -1; // SW_PIN2 が押されたら 0、押されてなければ -1

  if (value1 != -1 || value2 != -1) { // どちらかのボタンが押されたら
    int bit = (value1 != -1) ? value1 : value2; // 1 か 0 を取得
    data = (data << 1) | bit; // データを左シフトしてビットを追加
    bitCount++;

    if (bitCount == 8) { // 8ビット揃ったら
      Serial.print("Received ASCII: ");
      Serial.println((char)data); // ASCII 文字として出力
      data = 0; // データリセット
      bitCount = 0;
    }

    delay(300); // チャタリング防止のため少し待つ
  }

  delay(10); // CPU負荷軽減
}
