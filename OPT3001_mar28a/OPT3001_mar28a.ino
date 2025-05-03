#include <ESP32_NOW.h>
#include <ESP32_NOW_Serial.h>
#include <Wire.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_sleep.h"
#include "H:/Capstone/ESP_NOW_Test/myData.h"
#define Motion 15       //Motion sensor is at pin 15
float Lux;              //Amount of lux from light sensor

float SensorOpt3001_convert(uint16_t RawData)     //Function to calculate lux value
{
  uint16_t Exponent, Significand;
  Significand = RawData & 0x0FFF;                 // Extract Mantissa
  Exponent = (RawData & 0xF000) >> 12;            // Extract Exponent
  return Significand * (0.01 * pow(2, Exponent)); // Calculate final LUX
}

void setup() {
  // put your setup code here, to run once:
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(Motion, INPUT); //Set motion sensor to be an input

  Wire.begin();                   //Initialize in I2C master.
  Wire.beginTransmission(0x44);   //I2C address of OPT3001 = 0x44
  Wire.write(0x01);               //Config register address 0x01
  Wire.write(0xCE);
  Wire.write(0x10);               //Write 0xCE10 to turn on sensor
  Wire.endTransmission();         //End transmission

}

void loop() {
  // put your main code here, to run repeatedly:
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

    Serial.print("Lux: ");
    Serial.println(Lux);
    Serial.print("Motion: ");
    Serial.println(digitalRead(Motion));
    delay(1000);
  }

}
