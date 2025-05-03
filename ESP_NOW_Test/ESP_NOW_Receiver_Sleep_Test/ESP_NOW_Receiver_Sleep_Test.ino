#include <ESP32_NOW.h>
#include <ESP32_NOW_Serial.h>

#include <WiFi.h>
#include "esp_sleep.h"
#include "esp_wifi.h"
#include "H:/Capstone/ESP_NOW_Test/myData.h"


struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  //Serial.print("Int: ");
  //Serial.println(myData.b);
  Serial.print("Lux: ");
  Serial.println(myData.lightValue);
  //Serial.print("Bool: ");
  //Serial.println(myData.d);
  Serial.println();
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_sleep_enable_timer_wakeup(1000000);
  //esp_sleep_enable_wifi_wakeup();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {
  Serial.println("Awake");
  delay(1000);
  esp_wifi_stop();
  //delay(1000);
  esp_light_sleep_start();
  esp_wifi_start();

}
