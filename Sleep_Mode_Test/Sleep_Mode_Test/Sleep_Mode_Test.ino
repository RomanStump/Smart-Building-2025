#include <Wire.h>
#include "driver/timer.h"
#include "esp_sleep.h"
#define LED 25
#define Motion 15
float fLux;
int x = 0;

hw_timer_t *timer0 = NULL;


void IRAM_ATTR LEDOn()
{
  Serial.println("timer interrupt");
  //digitalWrite(LED, !digitalRead(LED));
  if (digitalRead(LED)) {
    digitalWrite(LED, LOW);
  }
}



void setup()
{
  Serial.begin(9600);                            // Initialize serial communication at 9600
  Wire.begin();                                  // Initialize Arduino in I2C master.
  Wire.beginTransmission(0x44);                  // I2C address of OPT3001 = 0x44
  Wire.write(0x01);                              // Config register address 0x01
  Wire.write(0xCE);
  Wire.write(0x10);                              // Write 0xCE10 to turn on sensor
  Wire.endTransmission();
  Serial.println("Data received \t\t Lux");
  pinMode(Motion, INPUT);
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(Motion), triggeredMotion, RISING);
  timer0 = timerBegin(100000);
  //timerStop(timer0);
  timerAttachInterrupt(timer0, LEDOn);
  timerWrite(timer0, 0);
  timerAlarm(timer0, 6000000, true, 0);
  gpio_wakeup_enable(GPIO_NUM_15, GPIO_INTR_HIGH_LEVEL);
  esp_sleep_enable_gpio_wakeup();


}
void loop()
{
  if (!digitalRead(LED)) {
    esp_light_sleep_start();
  }
  while (x == 1) {
    Serial.print("\nMotion:\t");
    Serial.print(digitalRead(Motion));
    Serial.print("\n");
    Wire.beginTransmission(0x44);
    Wire.write(0x00);                              // Send result register address
    Wire.endTransmission();
    delay(100);

    Wire.requestFrom(0x44, 2);                     // Request 2 bytes data from OPT3001
    uint16_t iData;
    uint8_t  iBuff[2];
    while (Wire.available())
    {
      Wire.readBytes(iBuff, 2);
      iData = (iBuff[0] << 8) | iBuff[1];
      //Serial.print(iData,BIN);                     // Print the received data
      //Serial.print("\t\t");
      fLux = SensorOpt3001_convert(iData);         // Calculate LUX from sensor data
      Serial.print("lux:\t");
      Serial.print(fLux);                        // Print it on serial terminal
    }
    if (fLux < 100) {
      //triggeredMotion();
      digitalWrite(LED, HIGH);
      x = 0;
      timerRestart(timer0);
    }
    x = 0;
    delay(900);
    //digitalWrite(LED, LOW);
  }
  int val = timerReadSeconds(timer0);
  Serial.print("\nTimer:\t");
  Serial.print(val);
  Serial.print("\n");
  delay(2000);
}

float SensorOpt3001_convert(uint16_t iRawData)
{
  uint16_t iExponent, iMantissa;
  iMantissa = iRawData & 0x0FFF;                 // Extract Mantissa
  iExponent = (iRawData & 0xF000) >> 12;         // Extract Exponent
  return iMantissa * (0.01 * pow(2, iExponent)); // Calculate final LUX
}

void triggeredMotion() {
  x = 1;
  //if (fLux < 100) {
  //Serial.println(digitalRead(Motion));
  //Serial.println(fLux);
  //digitalWrite(LED, HIGH);
  //timerRestart(timer0);
  //}
}
