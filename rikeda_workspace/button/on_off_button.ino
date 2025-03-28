#include <driver/gpio.h>

// GPIOピンの17番とタクトスイッチを接続しているため
const int SW_PIN = 17;

void setup() {
  pinMode( SW_PIN, INPUT);
  // INFO GPIO_NUM_XXX(XXは使用しているGPIOピン番号で置き換え)
  gpio_set_pull_mode( GPIO_NUM_17, GPIO_PULLUP_ONLY );

  Serial.begin( 115200 );
}

void loop() {
  if ( digitalRead( SW_PIN ) == LOW) {
    Serial.println( "ON." );
  } else {
    Serial.println( "OFF." );
  }
  delay( 1000 );
}
