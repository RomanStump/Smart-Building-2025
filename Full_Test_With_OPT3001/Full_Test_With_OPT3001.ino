#include <Wire.h>
#include "driver/timer.h"
#define LED 25
#define Motion 15
float Lux;
int motionDetected = 0;

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

}
void loop()
{
  while (motionDetected == 1) {
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
      Lux = SensorOpt3001_convert(iData);         // Calculate LUX from sensor data
      Serial.print("lux:\t");
      Serial.print(Lux);                        // Print it on serial terminal
    }
    if (Lux < 100) {
      //triggeredMotion();
      digitalWrite(LED, HIGH);
      timerRestart(timer0);
    }
    int val = timerReadSeconds(timer0);
    Serial.print("\nTimer:\t");
    Serial.print(val);
    Serial.print("\n");
    motionDetected=0;
    delay(900);
    //digitalWrite(LED, LOW);
  }
}

float SensorOpt3001_convert(uint16_t iRawData)
{
  uint16_t iExponent, iMantissa;
  iMantissa = iRawData & 0x0FFF;                 // Extract Mantissa
  iExponent = (iRawData & 0xF000) >> 12;         // Extract Exponent
  return iMantissa * (0.01 * pow(2, iExponent)); // Calculate final LUX
}

void triggeredMotion() {
  motionDetected = 1;
  //if (Lux < 100) {
  //Serial.println(digitalRead(Motion));
  //Serial.println(Lux);
  //digitalWrite(LED, HIGH);
  //timerRestart(timer0);
  //}
}
