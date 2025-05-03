#include <Wire.h>
#define LED 2
#define Light 4
#define Motion 15


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(0x44);
  Wire.write(0x01);                              // Config register address 0x01
  Wire.write(0x76);
  Wire.write(0x04);                              // Write 0x7604 to turn on sensor, example used 0xCE10
  Wire.endTransmission();
  Serial.println("Data received \t\t Lux");
}

void loop() {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(0x44);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom(0x44, 2);                     // Request 2 bytes data from OPT3001
  uint16_t iData;
  uint8_t  iBuff[2];
  while (Wire.available()) 
  { 
    Wire.readBytes(iBuff, 2);
    iData = (iBuff[0] << 8) | iBuff[1];
    Serial.print(iData,BIN);                     // Print the received data
    Serial.print("\t\t");
    float fLux = SensorOpt3001_convert(iData);   // Calculate LUX from sensor data
    Serial.println(fLux);                        // Print it on serial terminal
  }
  delay(1000);
}
float SensorOpt3001_convert(uint16_t iRawData)
{
  uint16_t iExponent, iMantissa;
  iMantissa = iRawData & 0x0FFF;                 // Extract Mantissa
  iExponent = (iRawData & 0xF000) >> 12;         // Extract Exponent 
  return iMantissa * (0.01 * pow(2, iExponent)); // Calculate final LUX
}

//Second example code that was tried
#include <Wire.h>
#include <ClosedCube_OPT3001.h> //Doesn't know what this is, can't find directory for it

ClosedCube_OPT3001 opt3001;

#define OPT3001_ADDRESS 0x44

void setup()
{
  Serial.begin(9600);
  Serial.println("ClosedCube OPT3001 Arduino Test");

  opt3001.begin(OPT3001_ADDRESS);
  Serial.print("OPT3001 Manufacturer ID");
  Serial.println(opt3001.readManufacturerID());
  Serial.print("OPT3001 Device ID");
  Serial.println(opt3001.readDeviceID());

  configureSensor();
  printResult("High-Limit", opt3001.readHighLimit());
  printResult("Low-Limit", opt3001.readLowLimit());
  Serial.println("----");
}

void loop()
{
  OPT3001 result = opt3001.readResult();
  printResult("OPT3001", result);
  delay(500);
}

void configureSensor() {
  OPT3001_Config newConfig;
  
  newConfig.RangeNumber = B1100;  
  newConfig.ConvertionTime = B0;
  newConfig.Latch = B1;
  newConfig.ModeOfConversionOperation = B11;

  OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
  if (errorConfig != NO_ERROR)
    printError("OPT3001 configuration", errorConfig);
  else {
    OPT3001_Config sensorConfig = opt3001.readConfig();
    Serial.println("OPT3001 Current Config:");
    Serial.println("------------------------------");
    
    Serial.print("Conversion ready (R):");
    Serial.println(sensorConfig.ConversionReady,HEX);

    Serial.print("Conversion time (R/W):");
    Serial.println(sensorConfig.ConvertionTime, HEX);

    Serial.print("Fault count field (R/W):");
    Serial.println(sensorConfig.FaultCount, HEX);

    Serial.print("Flag high field (R-only):");
    Serial.println(sensorConfig.FlagHigh, HEX);

    Serial.print("Flag low field (R-only):");
    Serial.println(sensorConfig.FlagLow, HEX);

    Serial.print("Latch field (R/W):");
    Serial.println(sensorConfig.Latch, HEX);

    Serial.print("Mask exponent field (R/W):");
    Serial.println(sensorConfig.MaskExponent, HEX);

    Serial.print("Mode of conversion operation (R/W):");
    Serial.println(sensorConfig.ModeOfConversionOperation, HEX);

    Serial.print("Polarity field (R/W):");
    Serial.println(sensorConfig.Polarity, HEX);

    Serial.print("Overflow flag (R-only):");
    Serial.println(sensorConfig.OverflowFlag, HEX);

    Serial.print("Range number (R/W):");
    Serial.println(sensorConfig.RangeNumber, HEX);

    Serial.println("------------------------------");
  }
  
}

void printResult(String text, OPT3001 result) {
  if (result.error == NO_ERROR) {
    Serial.print(text);
    Serial.print(": ");
    Serial.print(result.lux);
    Serial.println(" lux");
  }
  else {
    printError(text,result.error);
  }
}

void printError(String text, OPT3001_ErrorCode error) {
  Serial.print(text);
  Serial.print(": [ERROR] Code #");
  Serial.println(error);
}
