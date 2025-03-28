#include <driver/gpio.h>

const int UP_PIN = 17;
const int DOWN_PIN = 18;

int age = 30;

void setup() {
  pinMode(UP_PIN, INPUT);
  gpio_set_pull_mode(GPIO_NUM_17, GPIO_PULLUP_ONLY);

  pinMode(DOWN_PIN, INPUT);
  gpio_set_pull_mode(GPIO_NUM_18, GPIO_PULLUP_ONLY);

  Serial.begin(115200);
}

void loop() {
  int is_up = digitalRead(UP_PIN) == LOW ? 1 : 0;
  int is_down = digitalRead(DOWN_PIN) == LOW ? 1 : 0;

  if (is_up || is_down) { // どちらかのボタンが押されたら
    if (is_up && age < 130) {
      age += 1;
    } else if (is_down && age > 20) {
      age -= 1;
    }
    Serial.print("Age: ");
    Serial.println(age);
    delay(300); // チャタリング防止のため少し待つ
  }

  delay(10); // CPU負荷軽減
}
