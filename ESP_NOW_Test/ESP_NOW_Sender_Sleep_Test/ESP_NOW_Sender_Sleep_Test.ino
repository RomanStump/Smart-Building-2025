#include <ESP32_NOW.h>
#include <ESP32_NOW_Serial.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_sleep.h"
#include "H:/Capstone/ESP_NOW_Test/myData.h"
#define Motion 15

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x4C, 0x11, 0xAE, 0xDF, 0x8F, 0xBC};


struct_message myData;
bool delivery = true;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if(status == ESP_NOW_SEND_SUCCESS){
    delivery = true;
  }
  else{
    delivery = false;
  }
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(Motion, INPUT);
  gpio_wakeup_enable(GPIO_NUM_15, GPIO_INTR_HIGH_LEVEL);
  esp_sleep_enable_gpio_wakeup();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);


  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {

  //delay(1000);
  Serial.println(digitalRead(Motion));
  if (delivery) {
    esp_wifi_stop();
    esp_light_sleep_start();
  }
  //delay(1000);
  esp_wifi_start();

  myData.lightValue = 120.8;
  //myData.c = 1.2;
  //myData.d = false;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(1000);
}
