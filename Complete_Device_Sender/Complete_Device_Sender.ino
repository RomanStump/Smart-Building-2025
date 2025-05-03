#include <ESP32_NOW.h>
#include <ESP32_NOW_Serial.h>
#include <Wire.h>
#include <WiFi.h>
#include "esp_wifi.h"
//#include "esp_sleep.h"
#include "H:/Capstone/ESP_NOW_Test/myData.h"
#define Motion 15       //Motion sensor is at pin 15
float Lux;              //Amount of lux from light sensor
bool motionDetected = false;  //Has motion been detected
uint8_t broadcastAddress[] = {0xF4, 0x65, 0x0B, 0x4A, 0x24, 0x38}; //MAC address of receiver ESP32
bool delivery = true;         //Was the delivery of data successful
struct_message myData;        //Data being sent
esp_now_peer_info_t peerInfo; //Info of receiver ESP32 (peer ESP32)


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {  //When data is sent enter function
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");  //Print whether or not the delivery was a success
  if (status == ESP_NOW_SEND_SUCCESS) {
    delivery = true;    //If delivery was successful, "delivery" is true
  }
  else {
    delivery = false;   //Else "delivery" is false
  }
}

float SensorOpt3001_convert(uint16_t RawData)     //Function to calculate lux value
{
  uint16_t Exponent, Significand;
  Significand = RawData & 0x0FFF;                 // Extract Mantissa
  Exponent = (RawData & 0xF000) >> 12;            // Extract Exponent
  return Significand * (0.01 * pow(2, Exponent)); // Calculate final LUX
}

void IRAM_ATTR triggeredMotion() {  //Interrupt function for when motion sensor is triggered
  motionDetected = true;     //Motion has been detected
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(Motion, INPUT); //Set motion sensor to be an input

  gpio_wakeup_enable(GPIO_NUM_15, GPIO_INTR_HIGH_LEVEL);  //Set pin 15 to be the wake up pin, wakes up when that pin goes high (motion is triggered)
  esp_sleep_enable_gpio_wakeup();   //Enable GPIO to be the wakeup event

  WiFi.mode(WIFI_STA);   //Set device as a Wi-Fi Station

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //Register "OnDataSent" function to be the callback function that runs when data is sent
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6); //Copys broadcast address to peer
  peerInfo.channel = 0;     //Set channel
  peerInfo.encrypt = false; //Data is not encrypted

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Wire.begin();                   //Initialize in I2C master.
  Wire.beginTransmission(0x44);   //I2C address of OPT3001 = 0x44
  Wire.write(0x01);               //Config register address 0x01
  Wire.write(0xCE);
  Wire.write(0x10);               //Write 0xCE10 to turn on sensor
  Wire.endTransmission();         //End transmission

  //Register pin 15 to be the interrupt pin, "triggeredMotion" to be the interrupt function, and a rising edge sets off the interrupt
  attachInterrupt(digitalPinToInterrupt(Motion), triggeredMotion, RISING);
}

void loop() {

  if (delivery) {
    esp_wifi_stop();              //Turn off wifi to allow sleep mode
    delay(1000);
    esp_light_sleep_start();      //Enter light sleep
  }

  esp_wifi_start();               //Turn on wifi to allow communication

  if (motionDetected) {
    Wire.beginTransmission(0x44);   //Begin transmission with OPT3001
    Wire.write(0x00);               //Queue result register address
    Wire.endTransmission();         //Send data that was queued by wire.write() and end transmission

    Wire.requestFrom(0x44, 2);      //Request 2 bytes data from OPT3001
    uint16_t Data;
    uint8_t  Buff[2];

    while (Wire.available())
    {
      Wire.readBytes(Buff, 2);              //Read 2 bytes of data from Buff
      Data = (Buff[0] << 8) | Buff[1];      //
      Lux = SensorOpt3001_convert(Data);   //Calculate LUX from sensor data
    }

    myData.Movement = true;     //Let receiver know motion was detected
    myData.lightValue = Lux;    //Send light value to receiver
    Serial.print("Lux value: ");
    Serial.println(Lux);

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)); //Send message via ESP-NOW

    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }

    motionDetected = false;   //Return "motionDetected" back to 0
    delay(1000);
  }
}
